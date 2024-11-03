#include "DataManager.h"

using namespace omnetpp;
using namespace s2f::data;

Define_Module(DataManager);

/*template <class T>
T *findInMapOrNull(const std::map<opp_string, T> &map, const std::string &name)
{
    auto iter = map.find(name.c_str());
    return iter != map.end() ? &iter.second : nullptr;
}*/

void DataManager::initialize()
{
    // Get the experiment name
    experiment = par("experiment").stdstringValue();

    // Locate the DataEngine module
    cModule *module = getModuleByPath("^.engine");
    engine = check_and_cast<DataEngine *>(module);

    // Locate the DataRepository module
    module = getModuleByPath("^.repository");
    repository = check_and_cast<DataRepository *>(module);
}

void DataManager::finish()
{
    //
}

const Application *DataManager::searchApp(const std::string &name)
{
    Application *data = findInMapOrNull(repository->appMap, name);
    if (data)
        return data;
    return engine->searchApp(name);
}

const VirtualMachine *DataManager::searchVm(const std::string &name)
{
    VirtualMachine *data = findInMapOrNull(repository->vmMap, name);
    if (data)
        return data;
    return engine->searchVm(name);
}

const Sla *DataManager::searchSla(const std::string &name)
{
    Sla *data = findInMapOrNull(repository->slaMap, name);
    if (data)
        return data;
    return engine->searchSla(name);
}

const CloudUserPriority *DataManager::searchUser(const std::string &userType)
{
    CloudUserPriority *data = findInMapOrNull(repository->userMap, userType);
    if (data)
        return data;
    return engine->searchUser(userType, experiment);
}

std::unique_ptr<std::vector<const CloudUserPriority *>> DataManager::getSimUsers()
{
    engine->loadUsersInExperiment(experiment);
    
    // Allocate the return vector
    auto userVector = std::unique_ptr<std::vector<const CloudUserPriority *>>(new std::vector<const CloudUserPriority *>());

    for (const auto &userIterator : repository->userMap)
    {
        // Everything went well
        userVector->push_back(&userIterator.second);
    }

    return userVector;
}
