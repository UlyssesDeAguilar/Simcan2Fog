#include "DataManagerQuerier.h"
Define_Module(DataManagerQuerier);

using namespace s2f::data;
#define QUERY_START 1

void DataManagerQuerier::initialize()
{
    dm = check_and_cast<DataManager *>(getModuleByPath("simData.manager"));
    auto start = new cMessage("Entrypoint", QUERY_START);
    scheduleAt(0.0, start);
}

void DataManagerQuerier::finish()
{
}

void DataManagerQuerier::testApp(const std::string &appName)
{
    EV << "Search App with name: " << appName << "\n";
    auto app = dm->searchApp(appName);
    if (app)
        EV << *app << "\n";
    else
        EV << "Application: " << appName << " not found\n";
}

void DataManagerQuerier::testVM(const std::string &vmType)
{
    EV << "Search VM with name: " << vmType << "\n";
    auto vm = dm->searchVm(vmType);
    if (vm)
        EV << *vm << "\n";
    else
        EV << "VM: " << vmType << " not found\n";
}

void DataManagerQuerier::testSLA(const std::string &name)
{
    EV << "Search SLA with name: " << name << "\n";
    auto sla = dm->searchSla(name);
    if (sla)
        EV << *sla << "\n";
    else
        EV << "SLA: " << name << " not found\n";
}

void DataManagerQuerier::handleMessage(cMessage *msg)
{
    // Get kind and delete message
    auto kind = msg->getKind();
    delete msg;

    // Sanity check
    if (kind != QUERY_START)
        error("The impossible happened!");

    // Get the module (expected network is Unconnected)
    cModule *dmModule = getModuleByPath("simData.manager");
    if (dmModule == nullptr)
        error("Could not find simschema... selected network is Unconnected?");

    // Casting
    dm = dynamic_cast<DataManager *>(dmModule);
    if (dm == nullptr)
        error("Wrong class -> Doesn't inherit from SimSchema");

    // Queries!
    testApp("AppCPUIntensive");
    testVM("VM_4xlarge");
    testSLA("Slai0d0c0");

    // This should fail
    testApp("No-app");
}