#include "MySqlDataEngine.h"

using namespace s2f::data;
using namespace omnetpp;

Define_Module(MySqlDataEngine);

const MySqlDataEngine::QueryDefinitionArray MySqlDataEngine::queriesDefinition = {
    // APP_DATA
    R"(SELECT apps.name, parameters, app_models.name as type, package, interface 
    FROM apps JOIN app_models ON apps.model_id = app_models.id
    WHERE apps.name = ?)",
    // VM_DATA
    "SELECT type,cost,cores,scu,memory,disk FROM vms WHERE type = ?",
    // SLA_DATA
    R"(SELECT vms.type, sla_vms.cost, increase, discount, compensation
    FROM sla JOIN sla_vms ON sla.id = sla_id JOIN vms ON vm_id = vms.id 
    WHERE sla.type = ?)",
    // VM_COST
    R"(SELECT sla_vms.cost, increase, discount, compensation
    FROM sla JOIN sla_vms ON sla.id = sla_id JOIN vms ON vm_id = vms.id 
    WHERE sla.type = ? AND vms.type = ?)",
    // USER_BASE_INFO
    R"(
SELECT
    users.name, users.priority, sla.type as sla_type, experiment_users.instances
FROM
    experiment
    JOIN experiment_users ON experiment.id = experiment_users.experiment_id
    JOIN users ON experiment_users.user_id = users.id
    JOIN sla ON users.sla_id = sla.id
WHERE
    users.name = ? AND experiment.name = ?)",
    // SIM_USERS
    R"(
SELECT
    users.name, users.priority, sla.type as sla_type, experiment_users.instances
FROM
    experiment
    JOIN experiment_users ON experiment.id = experiment_users.experiment_id
    JOIN users ON experiment_users.user_id = users.id
    JOIN sla ON users.sla_id = sla.id
WHERE
    experiment.name = ?)",
    // APP_INFO
    R"(
SELECT 
    user_apps.instances, apps.name
FROM
    users
    JOIN user_apps ON users.id = user_apps.user_id
    JOIN apps ON apps.id = user_apps.app_id
WHERE
    users.name = ?)",
    // VM_INFO
    R"(
SELECT
    user_vms.instances, user_vms.rent_time, vms.type
FROM
    users
    JOIN user_vms ON users.id = user_vms.user_id
    JOIN vms ON vms.id = user_vms.vm_id
WHERE
    users.name = ?)"};

// Abbreviation for convenience
using nlohmann::json;

void MySqlDataEngine::initialize()
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

    DataEngine::initialize();
}

void MySqlDataEngine::finish()
{
    // Delete the prepared statements
    deleteStatements();
}

void MySqlDataEngine::deleteStatements()
{
    // Do not delete std::arrays like dynamic arrays (std::vector)!
    for (int i = 0; i < Queries::NUM_QUERIES; i++)
        delete queries[i];
}

void MySqlDataEngine::createStatements()
{
    for (int i = 0; i < Queries::NUM_QUERIES; i++)
        queries[i] = connection->prepareStatement(queriesDefinition[i]);
}

void MySqlDataEngine::attemptReconnect()
{
    // As the connection failed release the old statements
    deleteStatements();

    // Actual attempt to reconnect
    if (!connection->reconnect())
        throw sql::SQLException("Unable to reconnect with the database !");

    // Reload the prepared statements
    createStatements();
}

std::unique_ptr<sql::ResultSet>
MySqlDataEngine::executeStatement(Queries selectedQuery, std::function<void(sql::PreparedStatement *)> binder)
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

Application *MySqlDataEngine::searchApp(const std::string &name)
{
    // Notify the kernel
    Enter_Method_Silent("Query App : %s\n", name.c_str());

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
    auto path = res->getString("package");

    Application appTemplate(name, type, path);
    Application &app = repository->insertInMap(appName.c_str(), appTemplate);

    // Parse parameters from JSON
    json parameters = json::parse(res->getString("parameters").asStdString());
    json schema = json::parse(res->getString("interface").asStdString());

    // Construct the schema
    for (auto const &param : parameters.items())
    {
        try
        {
            // auto newParam = new AppParameter(param.key(), param.value(), value);
            std::string value = param.value();
            app.insertParameter(param.key().c_str(), value.c_str());
        }
        catch (const std::out_of_range &e)
        {
            EV_DETAIL << "Aplication" << name << "parameter: " << param.key()
                      << "not specified for model" << type << "\n";
        }
        // FIXME: CATCH POSSIBLE cRunTimeError from erroneous field expression or type !
    }

    // Return the new application schema
    return &app;
}

VirtualMachine *MySqlDataEngine::searchVm(const std::string &name)
{
    // Notify the kernel
    Enter_Method_Silent("Query VM : %s\n", name.c_str());

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
    auto cores = res->getInt("cores");
    auto scu = res->getDouble("scu");
    auto disk = res->getDouble("disk");
    auto memory = res->getDouble("memory");

    VirtualMachine vmTemplate(type, cost, cores, scu, disk, memory);
    VirtualMachine &vm = repository->insertInMap(type.c_str(), vmTemplate);

    return &vm;
}

Sla *MySqlDataEngine::searchSla(const std::string &name)
{
    // Notify the kernel
    Enter_Method_Silent("Query SLA : %s\n", name.c_str());

    // Bind and run query
    auto binder = [name](sql::PreparedStatement *s)
    {
        s->setString(1, name);
    };
    auto res = executeStatement(Queries::SLA_DATA, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return nullptr;

    Sla slaTemplate(name);
    Sla &sla = repository->insertInMap(name.c_str(), slaTemplate);

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
        sla.addVmCost(type, base, increase, discount, compensation);
    }

    return &sla;
}

CloudUserPriority *MySqlDataEngine::searchUser(const std::string &userType, const std::string &experiment)
{
    // Get the basic information about the user
    auto information = searchUserBaseInfo(userType, experiment);
    if (!information.ok)
        return nullptr;

    UserBaseInfo base = information.value;

    CloudUserPriority userTemplate(base.userName, base.numInstances, base.priority, base.slaId);
    CloudUserPriority &user = repository->insertInMap(base.userName.c_str(), userTemplate);

    loadUserAppsAndVms(&user);

    return &user;
}

MySqlDataEngine::DataReturn<MySqlDataEngine::UserBaseInfo>
MySqlDataEngine::searchUserBaseInfo(const std::string &user, const std::string &experiment)
{
    // Bind and run query
    auto binder = [user, experiment](sql::PreparedStatement *s)
    {
        s->setString(1, user);
        s->setString(2, experiment);
    };
    auto res = executeStatement(Queries::USER_BASE_INFO, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return {.ok = false};

    // Load row and retrieve the basic info
    res->next();

    UserBaseInfo base;
    base.userName = res->getString("name").asStdString();
    base.numInstances = res->getInt("instances");
    base.slaId = res->getString("sla_type").asStdString();
    base.priority = res->getInt("priority") == 1 ? tPriorityType::Priority : tPriorityType::Regular;

    return {.ok = true, .value = base};
}

void MySqlDataEngine::loadUsersInExperiment(const std::string &experiment)
{
    // Notify the kernel
    Enter_Method_Silent("Query users in experiment: %s", experiment.c_str());

    // Bind and run query
    auto binder = [experiment](sql::PreparedStatement *s)
    {
        s->setString(1, experiment);
    };
    auto res = executeStatement(Queries::SIM_USERS, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        error("No users in current experiment");

    // Load rows and retrieve the basic info
    while (res->next())
    {
        // Allocate in place the element
        const char *userName = res->getString("name").c_str();
        CloudUserPriority userTemplate(userName,
                                       res->getInt("instances"),
                                       res->getInt("priority") == 1 ? tPriorityType::Priority : tPriorityType::Regular,
                                       res->getString("sla_type").c_str());
        
        CloudUserPriority &user = repository->insertInMap(userName, userTemplate);

        loadUserAppsAndVms(&user);
    }

    return;
}

void MySqlDataEngine::loadUserAppsAndVms(CloudUserPriority *user)
{
    // FIXME
    // For all vms
    auto vmCursor = searchUserVms(user->getType());
    while (vmCursor->next())
    {
        auto vm = searchVm(vmCursor->getString("type"));
        if (vm == nullptr)
            error("EINVAL");

        user->insertVirtualMachine(vm, vmCursor->getInt("instances"), vmCursor->getInt("rent_time"));
    }

    // For all apps
    auto appCursor = searchUserApps(user->getType());
    while (appCursor->next())
    {
        auto app = searchApp(appCursor->getString("name"));
        if (app == nullptr)
            error("EINVAL");

        user->insertApplication(app, appCursor->getInt("instances"));
    }
}

std::unique_ptr<sql::ResultSet>
MySqlDataEngine::searchUserApps(const std::string &user)
{
    // Notify the kernel
    Enter_Method_Silent("Query requested apps of user: %s", user.c_str());

    // Bind and run query
    auto binder = [user](sql::PreparedStatement *s)
    {
        s->setString(1, user);
    };
    auto res = executeStatement(Queries::APP_INFO, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return nullptr;
    else
        return res;
}

std::unique_ptr<sql::ResultSet>
MySqlDataEngine::searchUserVms(const std::string &user)
{
    // Notify the kernel
    Enter_Method_Silent("Query requested vms of user: %s", user.c_str());

    // Bind and run query
    auto binder = [user](sql::PreparedStatement *s)
    {
        s->setString(1, user);
    };
    auto res = executeStatement(Queries::VM_INFO, binder);

    // In the case we did not find any match or the query failed
    if (!res || res->rowsCount() == 0)
        return nullptr;
    else
        return res;
}