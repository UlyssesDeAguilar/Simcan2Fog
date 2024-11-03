#ifndef _SIMCAN_EX_DATA_REPOSITORY
#define _SIMCAN_EX_DATA_REPOSITORY

#include <omnetpp.h>
#include <memory>

// Data Classes that represent the information
#include "s2f/management/dataClasses/Applications/Application.h"
#include "s2f/management/dataClasses/SLAs/Sla.h"
#include "s2f/management/dataClasses/Users/CloudUser.h"
#include "s2f/management/dataClasses/Users/CloudUserPriority.h"
#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"

template <class T>
T *findInMapOrNull(std::map<opp_string, T> &map, const std::string &name)
{
    auto iter = map.find(name.c_str());
    return iter != map.end() ? &iter->second : nullptr;
}

class DataRepository : public omnetpp::cSimpleModule
{
public:
    template <class T>
    using DefinitionMap = std::map<omnetpp::opp_string, T>;

    DefinitionMap<Application> appMap;
    DefinitionMap<VirtualMachine> vmMap;
    DefinitionMap<Sla> slaMap;
    DefinitionMap<CloudUserPriority> userMap;

protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;

public:
    template <typename T>
    T &insertInMap(const char *name, const T &data);
};

#endif
