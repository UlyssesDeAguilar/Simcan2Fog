#include "SimSchemaQuerier.h"
Define_Module(SimSchemaQuerier);
using namespace simschema;
#define QUERY_START 1

void SimSchemaQuerier::initialize()
{
    auto start = new cMessage("Entrypoint", QUERY_START);
    scheduleAt(0.0, start);
}

void SimSchemaQuerier::finish()
{
}

void SimSchemaQuerier::testApp(const std::string &appName)
{
    EV << "Search App with name: " << appName << "\n";
    auto app = simSchema->searchApp(appName);
    if (app)
        EV << *app.get() << "\n";
    else
        EV << "Application: " << appName << " not found\n";
}

void SimSchemaQuerier::testVM(const std::string &vmType)
{
    EV << "Search VM with name: " << vmType << "\n";
    auto vm = simSchema->searchVirtualMachine(vmType);
    if (vm)
        EV << *vm.get() << "\n";
    else
        EV << "VM: " << vmType << " not found\n";
}

void SimSchemaQuerier::testSLA(const std::string &name)
{
    EV << "Search SLA with name: " << name << "\n";
    auto sla = simSchema->searchSLA(name);
    if (sla)
        EV << *sla.get() << "\n";
    else
        EV << "SLA: " << name << " not found\n";
}
void SimSchemaQuerier::testVMCost(const std::string &sla, const std::string &vmType)
{
    EV << "Search VMCost with name: " << vmType << " in SLA " << sla << "\n";
    auto vmCost = simSchema->searchVMCost(sla, vmType);
    if (vmCost)
        EV << *vmCost.get() << "\n";
    else
        EV << "VMCost: " << vmType << " SLA: " << sla <<" not found\n";
}

void SimSchemaQuerier::handleMessage(cMessage *msg)
{
    // Get kind and delete message
    auto kind = msg->getKind();
    delete msg;

    // Sanity check
    if (kind != QUERY_START)
        error("The impossible happened!");

    // Get the module (expected network is Unconnected)
    cModule *simSchemaModule = getModuleByPath("^.simschema");
    if (simSchemaModule == nullptr)
        error("Could not find simschema... selected network is Unconnected?");

    // Casting
    simSchema = dynamic_cast<SimSchema *>(simSchemaModule);
    if (simSchema == nullptr)
        error("Wrong class -> Doesn't inherit from SimSchema");

    // Queries!
    testApp("AppCPUIntensive");
    testVM("VM_4xlarge");
    testSLA("Slai0d0c0");
    testVMCost("Slai0d0c0", "VM_4xlarge");
    
    // Next one should fail
    testVMCost("", "");
}