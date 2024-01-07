#include "SimSchemaMySQL.h"
Define_Module(SimSchemaMySQL);

const SimSchemaMySQL::QueryDefinitionArray SimSchemaMySQL::queriesDefinition = {
    R"(SELECT apps.name, parameters, app_models.name as type, package, interface 
    FROM apps JOIN app_models ON apps.model_id = app_models.id
    WHERE apps.name = ?)",
    "SELECT type,cost,cores,scu,memory,disk FROM vms WHERE type = ?",
    R"(SELECT vms.type, sla_vms.cost, increase, discount, compensation
    FROM sla JOIN sla_vms ON sla.id = sla_id JOIN vms ON vm_id = vms.id 
    WHERE sla.type = ?)",
    R"(SELECT sla_vms.cost, increase, discount, compensation
    FROM sla JOIN sla_vms ON sla.id = sla_id JOIN vms ON vm_id = vms.id 
    WHERE sla.type = ? AND vms.type = ?)"
};

// Abbreviation for convenience
using nlohmann::json;

void SimSchemaMySQL::initialize()
{
    // Retrieve from the environement the password for the DB
    const char *password = std::getenv("S2F_DB_PASSWORD");
    if (password == NULL)
        error("Unable to connect to database: Missing password in environement");

    // Retrieve the host name and the user name
    hostName = par("hostName").stdstringValue();
    userName = par("userName").stdstringValue();

    try
    {
        // Get the driver instance and connect
        driver = get_driver_instance();
        connection = std::unique_ptr<sql::Connection>(driver->connect(hostName, userName, password));

        // Select the DB
        connection->setSchema("simcan2fog");

        // Prepare the statements
        createStatements();
    }
    catch (sql::SQLException &e)
    {
        EV_ERROR << "Connection failed for hostName: " << hostName << " user: " << userName << "\n";
        EV_ERROR << "Details: " << e.what();
        EV_ERROR << "Error Code:" << e.getErrorCode() << ", State: " << e.getSQLState() << std::endl;
        error("Connection or initialization failure");
    }
}

void SimSchemaMySQL::finish()
{
    // Delete the prepared statements
    deleteStatements();
}

void SimSchemaMySQL::deleteStatements()
{
    // Do not delete std::arrays like dynamic arrays (std::vector)!
    for (int i = 0; i < Queries::NUM_QUERIES; i++)
        delete queries[i];
}

void SimSchemaMySQL::createStatements()
{
    for (int i = 0; i < Queries::NUM_QUERIES; i++)
        queries[i] = connection->prepareStatement(queriesDefinition[i]);
}

void SimSchemaMySQL::attemptReconnect()
{
    // As the connection failed release the old statements
    deleteStatements();

    // Actual attempt to reconnect
    if(!connection->reconnect())
        throw sql::SQLException("Unable to reconnect with the database !");

    // Reload the prepared statements
    createStatements();
}

std::unique_ptr<sql::ResultSet>
SimSchemaMySQL::executeStatement(Queries selectedQuery, std::function<void(sql::PreparedStatement *)> binder)
{
    try
    {
        // Check valid connection
        if (!connection->isValid())
            attemptReconnect();

        // Select query, bind parameter and execute
        auto query = queries[selectedQuery];
        binder(query);
        return std::unique_ptr<sql::ResultSet>(query->executeQuery());
    }
    catch (sql::SQLException &e)
    {
        EV_ERROR << "Connection or execution failed for hostName: " << hostName << " user: " << userName << "\n";
        EV_ERROR << "Details: " << e.what();
        EV_ERROR << "Error Code:" << e.getErrorCode() << ", State: " << e.getSQLState() << std::endl;
        return nullptr;
    }
}

std::unique_ptr<Application>
SimSchemaMySQL::searchApp(const std::string &name)
{
    // Bind and run query
    auto binder = [name](sql::PreparedStatement *s)
    {
        s->setString(1, name);
    };
    auto res = executeStatement(Queries::APP_DATA, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return nullptr;

    // Load row and retrieve the basic info
    res->next();
    auto appName = res->getString("name");
    auto type = res->getString("type");
    auto package = res->getString("package");

    auto newApp = std::unique_ptr<Application>(new Application(appName, type, package));

    // Parse parameters from JSON
    json parameters = json::parse(res->getString("parameters").asStdString());
    json schema = json::parse(res->getString("interface").asStdString());

    // Construct the schema
    for (auto const &param : schema.items())
    {
        try
        {
            json value = parameters.at(param.key());

            auto newParam = new AppParameter(param.key(), param.value(), value);
            newApp->insertParameter(newParam);
        }
        catch (const std::out_of_range &e)
        {
            EV_DETAIL << "Aplication" << name << "parameter: " << param.key()
                      << "not specified for model" << type << "\n";
        }
        // FIXME: CATCH POSSIBLE cRunTimeError from erroneous field expression or type !
    }

    // Return the new application schema
    return newApp;
}

std::unique_ptr<VirtualMachine>
SimSchemaMySQL::searchVirtualMachine(const std::string &name)
{
    // Bind and run query
    auto binder = [name](sql::PreparedStatement *s)
    {
        s->setString(1, name);
    };
    auto res = executeStatement(Queries::VM_DATA, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return nullptr;

    // Load row and retrieve the basic info
    res->next();
    auto type = res->getString("type");
    auto cost = res->getDouble("cost");
    auto numCores = res->getInt("cores");
    auto scu = res->getDouble("scu");
    auto diskGB = res->getDouble("disk");
    auto memoryGB = res->getDouble("memory");

    auto vm = new VirtualMachine(type, cost, numCores, scu, diskGB, memoryGB);
    return std::unique_ptr<VirtualMachine>(vm);
}

std::unique_ptr<Sla>
SimSchemaMySQL::searchSLA(const std::string &name)
{
    // Bind and run query
    auto binder = [name](sql::PreparedStatement *s)
    {
        s->setString(1, name);
    };
    auto res = executeStatement(Queries::SLA_DATA, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return nullptr;

    auto newSla = std::unique_ptr<Sla>(new Sla(name));

    // While there are rows (VM definitions)
    while (res->next())
    {
        // VM relevant data
        auto type = res->getString("type");

        // Sla relevant data
        auto base = res->getDouble("cost");
        auto increase = res->getDouble("increase");
        auto discount = res->getDouble("discount");
        auto compensation = res->getDouble("compensation");

        // Insert the VM
        newSla->addVmCost(type, base, increase, discount, compensation);
    }

    return newSla;
}

std::unique_ptr<Sla::VMCost>
SimSchemaMySQL::searchVMCost(const std::string &sla, const std::string &vmType)
{
    // Bind and run query
    auto binder = [sla, vmType](sql::PreparedStatement *s)
    {
        s->setString(1, sla);
        s->setString(2, vmType);
    };
    auto res = executeStatement(Queries::VM_COST, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return nullptr;

    // Load row and retrieve the basic info
    res->next();

    auto vmCost = new Sla::VMCost({.base = static_cast<double>(res->getDouble("cost")),
                                   .increase = static_cast<double>(res->getDouble("increase")),
                                   .discount = static_cast<double>(res->getDouble("discount")),
                                   .compensation = static_cast<double>(res->getDouble("compensation"))});
    return std::unique_ptr<Sla::VMCost>(vmCost);
}
