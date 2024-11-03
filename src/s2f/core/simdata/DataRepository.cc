#include "DataRepository.h"
using namespace omnetpp;

Define_Module(DataRepository);

void DataRepository::initialize()
{
}

void DataRepository::handleMessage(cMessage *msg)
{
    error("This class doesn't recieve messages");
}

void DataRepository::finish()
{
    appMap.clear();
    vmMap.clear();
    slaMap.clear();
    userMap.clear();
}

template <>
Application& DataRepository::insertInMap(const char *name, const Application &data)
{
    // Allocate in data repository
    typename DataRepository::DefinitionMap<Application>::iterator iter;
    bool wasInserted;
    std::tie(iter, wasInserted) = appMap.emplace(name, data);

    if (!wasInserted)
        error("Error while inserting %s %s in DataRepository", name, "app");

    return iter->second;
}

template <>
VirtualMachine& DataRepository::insertInMap(const char *name, const VirtualMachine &data)
{
    // Allocate in data repository
    typename DataRepository::DefinitionMap<VirtualMachine>::iterator iter;
    bool wasInserted;
    std::tie(iter, wasInserted) = vmMap.emplace(name, data);

    if (!wasInserted)
        error("Error while inserting %s %s in DataRepository", name, "vm");

    return iter->second;
}

template <>
Sla& DataRepository::insertInMap(const char *name, const Sla &data)
{
    // Allocate in data repository
    typename DataRepository::DefinitionMap<Sla>::iterator iter;
    bool wasInserted;
    std::tie(iter, wasInserted) = slaMap.emplace(name, data);  

    if (!wasInserted)
        error("Error while inserting %s %s in DataRepository", name, "sla");

    return iter->second;
}

template <>
CloudUserPriority& DataRepository::insertInMap(const char *name, const CloudUserPriority &data)
{
    // Allocate in data repository
    typename DataRepository::DefinitionMap<CloudUserPriority>::iterator iter;
    bool wasInserted;
    std::tie(iter, wasInserted) = userMap.emplace(name, data);

    if (!wasInserted)    
        error("Error while inserting %s %s in DataRepository", name, "user");

    return iter->second;
}