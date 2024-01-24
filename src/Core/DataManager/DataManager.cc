#include "DataManager.h"

Define_Module(DataManager);
using omnetpp::cModule;

void DataManager::initialize()
{
    // Get the experiment name
    experiment = par("experiment").stdstringValue();

    // Locate the SimSchema module
    cModule *module = getModuleByPath("^.simSchema");
    simSchema = check_and_cast<simschema::SimSchema *>(module);
}

void DataManager::finish()
{
    // Clean chache
    appCache.clear();
    vmCache.clear();
    slaCache.clear();
    userCache.clear();
}

const Application *DataManager::searchApp(const std::string &name)
{
    try
    {
        return appCache.at(name).get();
    }
    catch (std::out_of_range &e)
    {
        // Should query the application type
        auto newApp = simSchema->searchApp(name);
        auto rawAdress = newApp.get();

        // If we got lucky
        if (newApp)
            appCache[name] = std::move(newApp);

        return rawAdress;
    }
}

const VirtualMachine *DataManager::searchVirtualMachine(const std::string &name)
{
    try
    {
        return vmCache.at(name).get();
    }
    catch (std::out_of_range &e)
    {
        // Should query the application type
        auto newVm = simSchema->searchVirtualMachine(name);
        auto rawAdress = newVm.get();

        // If we got lucky
        if (newVm)
            vmCache[name] = std::move(newVm);

        return rawAdress;
    }
}

const Sla *DataManager::searchSla(const std::string &name)
{
    try
    {
        return slaCache.at(name).get();
    }
    catch (std::out_of_range &e)
    {
        // Should query the application type
        auto newSla = simSchema->searchSLA(name);
        auto rawAdress = newSla.get();

        // If we got lucky
        if (newSla)
            slaCache[name] = std::move(newSla);

        return rawAdress;
    }
}

const Sla::VMCost *DataManager::searchVMCost(const std::string &sla, const std::string &vmType)
{
    try
    {
        return slaCache.at(sla)->getVmCost(vmType);
    }
    catch (std::out_of_range &e)
    {
        const Sla::VMCost *vmCost = nullptr;

        // Should query the application type
        auto newSla = simSchema->searchSLA(sla);

        // If we got lucky
        if (newSla)
        {
            vmCost = newSla->getVmCost(vmType);
            slaCache[sla] = std::move(newSla);
        }
        return vmCost;
    }
}

const CloudUserPriority *DataManager::searchUser(const std::string &userType)
{
    try
    {
        return userCache.at(userType).get();
    }
    catch (std::out_of_range &e)
    {
        auto basic = simSchema->searchUserBaseInfo(userType, experiment);
        if (!basic)
            return nullptr;

        // Call internal building
        auto user = searchUser(*basic);
        if (user == nullptr)
            return nullptr;

        // Everything went well, cache and return
        userCache[userType] = std::unique_ptr<const CloudUserPriority>(user);
        return user;
    }
}

const CloudUserPriority *DataManager::searchUser(const simschema::UserBaseInfo &base)
{
    // From basic info create the "skeleton"
    auto newUser = std::unique_ptr<CloudUserPriority>(
        new CloudUserPriority(base.userName, base.numInstances, base.priority, base.slaId));

    // For all vms
    auto vmsList = simSchema->searchUserVms(base.userName);
    for (auto &vmInfo : *vmsList)
    {
        auto vm = searchVirtualMachine(vmInfo.type);
        if (vm == nullptr)
            return nullptr;

        newUser->insertVirtualMachine(vm, vmInfo.numInstances, vmInfo.nRentTime);
    }

    // For all apps
    auto appsList = simSchema->searchUserApps(base.userName);
    for (auto &appInfo : *appsList)
    {
        auto app = searchApp(appInfo.appName);
        if (app == nullptr)
            return nullptr;

        newUser->insertApplication(app, appInfo.numInstances);
    }

    return newUser.release();
}

std::unique_ptr<std::vector<const CloudUserPriority *>> DataManager::getSimUsers()
{
    // Get the users list
    auto userBases = simSchema->searchUsersInExperiment(experiment);
    if (!userBases)
        error("Could not load users from experiment [%s]", experiment.c_str());

    // Allocate the return vector
    auto userVector = std::unique_ptr<std::vector<const CloudUserPriority *>>(new std::vector<const CloudUserPriority *>());

    for (const auto &userBase : *userBases)
    {
        auto newUser = searchUser(userBase);
        if (!newUser)
            error("Could not load user [%s]", userBase.userName.c_str());

        // Everything went well
        userVector->push_back(newUser);
        userCache[userBase.userName] = std::unique_ptr<const CloudUserPriority>(newUser);
    }

    return userVector;
}
