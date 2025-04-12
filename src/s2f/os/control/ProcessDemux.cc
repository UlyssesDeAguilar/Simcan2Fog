#include "s2f/os/control/ProcessDemux.h"
using namespace omnetpp;
using namespace hypervisor;

Register_Class(PidProcessDemux);
Register_Class(VmIdProcessDemux);

int PidProcessDemux::chooseIndex(omnetpp::cMessage *msg) 
{
    return check_and_cast<Syscall *>(msg)->getPid();
}

int VmIdProcessDemux::chooseIndex(omnetpp::cMessage *msg) 
{
    return check_and_cast<Syscall *>(msg)->getVmId();
}