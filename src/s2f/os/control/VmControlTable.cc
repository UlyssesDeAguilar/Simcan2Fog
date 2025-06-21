#include "s2f/os/control/VmControlTable.h"

using namespace omnetpp;
using namespace s2f::os;
Define_Module(VmControlTable);

void VmControlTable::initialize()
{
    // Prepare the control tables
    vmTable.resize(par("numVMs"));
    for (auto &entry : vmTable)
        entry.apps.resize(par("numAppsPerVm"));
}

void VmControlTable::finish()
{
    vmTable.clear();
    globalToLocalVmIdMap.clear();
    localToGlobalVmIdMap.clear();
}

void VmControlTable::handleMessage(omnetpp::cMessage *msg)
{
    delete msg;
    error("This module doesn't handle messages");
}

int VmControlTable::allocateVm(const VirtualMachine *vmType, const char *userId)
{
    // Search for a free vm
    for (int i = 0; i < vmTable.size(); i++)
    {
        if (vmTable[i].isFree())
        {
            vmTable[i].initialize(i, vmType, userId, par("numAppsPerVm"));
            vmTable[i].setRunning();
            return i;
        }
    }
    return -1;
}

int VmControlTable::allocateApp(int vmId, int deploymentIndex)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    ASSERT2(vmTable[vmId].isRunning(), "Invalid vmId, not running");

    for (int i = 0; i < vmTable[vmId].apps.size(); i++)
    {
        if (vmTable[vmId].apps[i].isFree())
        {
            vmTable[vmId].apps[i].initialize(vmId, i, deploymentIndex);
            return i;
        }
    }
    return -1;
}

void VmControlTable::deallocateVm(int vmId)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    ASSERT2(vmTable[vmId].isFree(), "Invalid vmId, not free");
    vmTable[vmId].free();
}

void VmControlTable::deallocateApp(int vmId, int appId)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    ASSERT2(appId >= 0 && appId < vmTable[vmId].apps.size(), "Invalid appId, out of bounds");
    vmTable[vmId].apps[appId].free();
}

void VmControlTable::suspendVm(int vmId)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    ASSERT2(vmTable[vmId].isRunning(), "Invalid vmId, not running");
    vmTable[vmId].setSuspended();
}

void VmControlTable::bufferCall(int vmId, cMessage *msg)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    ASSERT2(vmTable[vmId].isSuspended(), "Invalid vmId, not suspended");
    vmTable[vmId].callBuffer.insert(msg);
}

bool VmControlTable::vmIsSuspended(int vmId)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    return vmTable[vmId].isSuspended();
}

cQueue &VmControlTable::resumeVm(int vmId)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    ASSERT2(vmTable[vmId].isSuspended(), "Invalid vmId, not suspended");
    vmTable[vmId].setRunning();
    return vmTable[vmId].callBuffer;
}

void VmControlTable::createMapping(const char *globalVmId, int localVmId)
{
    ASSERT2(localVmId >= 0 && localVmId < vmTable.size(), "Invalid vmId, out of bounds");
    globalToLocalVmIdMap[globalVmId] = localVmId;
    localToGlobalVmIdMap[localVmId] = globalVmId;
}

const VmControlBlock & VmControlTable::getVmControlBlock(int vmId) const
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    return vmTable[vmId];
}

const AppControlBlock &VmControlTable::getAppControlBlock(int vmId, int appId) const
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    ASSERT2(appId >= 0 && appId < vmTable[vmId].apps.size(), "Invalid appId, out of bounds");
    return vmTable[vmId].apps[appId];
}

void VmControlTable::associateDeploymentToVm(int vmId, SM_UserAPP *request)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    take(request);
    vmTable[vmId].request = request;
}

int VmControlTable::getPidFromServiceName(int vmId, const char *serviceName)
{
    ASSERT2(vmId >= 0 && vmId < vmTable.size(), "Invalid vmId, out of bounds");
    return vmTable[vmId].request->findRequestIndex(serviceName);
}