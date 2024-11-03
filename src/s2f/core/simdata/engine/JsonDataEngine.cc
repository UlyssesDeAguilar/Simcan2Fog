#include "JsonDataEngine.h"

using namespace s2f::data;
using namespace omnetpp;
Define_Module(JsonDataEngine);

void JsonDataEngine::initialize()
{
    // Don't forget to initialize the base
    DataEngine::initialize();

    // Create a map for the users and their instances in the experiment
    std::map<omnetpp::opp_string, int> instancesMap;

    bool checkConsistency = par("checkConsistency");

    // Load the VMs
    loadVms();

    // Then, load the SLAs from the JSON definitions
    // with the option to check consistency with the VMs
    loadSlas(checkConsistency);

    // Load the applications
    loadApps();

    // Users are loaded when DataManager specifies which experiment to load!
}

void JsonDataEngine::finish()
{
}

void JsonDataEngine::handleMessage(omnetpp::cMessage *msg)
{
    delete msg;
    error("This module doesn't receive messages");
}

void JsonDataEngine::loadApps()
{
    cValueArray *appArray = check_and_cast<cValueArray *>(par("apps").objectValue());

    for (const auto &definition : appArray->asObjectVector<cValueMap>())
    {
        const char *name = definition->get("name");
        const char *type = definition->get("model");
        const char *path = definition->get("package");

        Application appTemplate(name, type, path);
        Application &app = repository->insertInMap(name, appTemplate);

        // Retrieving key:value parameters for the instance type
        auto paramMap = check_and_cast<cValueMap *>((cObject *)definition->get("parameters"));
        for (const auto &param : paramMap->getFields())
            app.insertParameter(param.first.c_str(), param.second);
    }
}

void JsonDataEngine::loadVms()
{
    cValueArray *vmArray = check_and_cast<cValueArray *>(par("vms").objectValue());

    for (const auto &definition : vmArray->asObjectVector<cValueMap>())
    {
        const char *name = definition->get("name");
        double cost = definition->get("cost");
        int cores = definition->get("cores");
        double scu = definition->get("scu");
        double memory = definition->get("memory");
        double disk = definition->get("disk");

        VirtualMachine vmTemplate(name, cost, cores, scu, disk, memory);
        repository->insertInMap(name, vmTemplate);
    }
}

void JsonDataEngine::loadSlas(bool checkConsistency)
{
    cValueArray *slaArray = check_and_cast<cValueArray *>(par("slas").objectValue());

    for (const auto &definition : slaArray->asObjectVector<cValueMap>())
    {
        const char *name = definition->get("name");
        Sla slaTemplate(name);
        Sla &sla = repository->insertInMap(name, slaTemplate);

        auto vmBinds = check_and_cast<cValueArray *>(definition->get("bindings").objectValue());
        for (const auto &vmBind : vmBinds->asObjectVector<cValueMap>())
        {
            const char *name = vmBind->get("name");
            auto base = vmBind->get("cost");
            auto increase = vmBind->get("increase");
            auto discount = vmBind->get("discount");
            auto compensation = vmBind->get("compensation");

            if (checkConsistency)
            {
                if (findInMapOrNull(repository->vmMap, name) == nullptr)
                    error("Cross reference integrity check failed for sla %s and vm binding %s",
                          name, name);
            }

            sla.addVmCost(name, base, increase, discount, compensation);
        }
    }
}

void JsonDataEngine::loadUsersInExperiment(const std::string &experiment)
{
    // Load the users from the parameter experiment
    auto experimentMap = check_and_cast<cValueMap *>(par("experiments").objectValue());
    auto experimentDefinition = check_and_cast<cValueMap *>(experimentMap->get(experiment.c_str()).objectValue());
    auto usersArray = check_and_cast<cValueArray *>(experimentDefinition->get("users").objectValue());

    // Load user definitions
    auto userDefinitions = check_and_cast<cValueMap *>(par("users").objectValue());

    for (const auto &definition : usersArray->asObjectVector<cValueMap>())
    {
        const char *userName = definition->get("name");
        int instances = definition->get("instances");

        // Search the user in the definitions
        auto userDefinition = check_and_cast<cValueMap *>(userDefinitions->get(userName).objectValue());

        tPriorityType priority = ((bool)userDefinition->get("priority")) == true ? Priority : Regular;
        const char *slaId = userDefinition->get("sla_id");

        CloudUserPriority userTemplate(userName, instances, priority, slaId);
        CloudUserPriority &user = repository->insertInMap(userName, userTemplate);

        // Insert the appropiate apps
        auto appArray = check_and_cast<cValueArray *>(userDefinition->get("apps").objectValue());
        for (const auto &app : appArray->asObjectVector<cValueMap>())
        {
            const char *name = app->get("name");
            int instances = app->get("instances");

            auto appDefinition = findInMapOrNull(repository->appMap, name);
            if (appDefinition == nullptr)
                error("User %s has an app %s that does not exist", userName, name);

            user.insertApplication(appDefinition, instances);
        }

        // Insert the appropiate vms
        auto vmArray = check_and_cast<cValueArray *>(userDefinition->get("vms").objectValue());
        for (const auto &vm : vmArray->asObjectVector<cValueMap>())
        {
            const char *name = vm->get("name");
            int instances = vm->get("instances");
            int rentTime = vm->get("rent_time");

            auto vmDefinition = findInMapOrNull(repository->vmMap, name);
            if (vmDefinition == nullptr)
                error("User %s has an vm %s that does not exist", userName, name);

            user.insertVirtualMachine(vmDefinition, instances, rentTime);
        }
    }
}

Application *JsonDataEngine::searchApp(const std::string &name)
{
    // Notify the kernel
    Enter_Method_Silent("Query App : %s\n", name.c_str());
    return nullptr;
}

VirtualMachine *JsonDataEngine::searchVm(const std::string &name)
{
    // Notify the kernel
    Enter_Method_Silent("Query VM : %s\n", name.c_str());
    return nullptr;
}

Sla *JsonDataEngine::searchSla(const std::string &name)
{
    // Notify the kernel
    Enter_Method_Silent("Query SLA : %s\n", name.c_str());
    return nullptr;
}

CloudUserPriority *JsonDataEngine::searchUser(const std::string &userType, const std::string &experiment)
{
    Enter_Method_Silent("Query User : %s\n", userType.c_str());
    return nullptr;
}