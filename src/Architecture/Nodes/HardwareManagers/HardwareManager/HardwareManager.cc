#include "HardwareManager.h"

Define_Module(HardwareManager);

void HardwareManager::initialize()
{

    int i;

    // Obtain the parameters of the module
    isVirtualHardware = par("isVirtualHardware");
    maxVMs = par("maxVMs");
    numAvailableCpuCores = numCpuCores = par("numCpuCores");
    availableMemory = memorySize = par("memorySize");
    availableDisk = diskSize = par("diskSize");
    maxUsers = par("maxUsers");
    freeCoresArrayPtr = new bool[numCpuCores];

    // Check consistency (cloud environments)
    if (isVirtualHardware)
    {

        if ((maxVMs <= 0) || (maxVMs > numCpuCores))
            error("For virtual environments, 0 < maxVMs <= numCpuCores");

        if (numCpuCores <= 0)
            error("The parameter numCpuCores must be a positive value (>0)");

        if (maxUsers != maxVMs)
            error("For virtual environments, maxUsers = maxVMs");
    }
    // Check consistency (cluster environments)
    else
    {
        if (maxVMs != 1)
            error("For non-virtual environments, maxVMs must be 1");

        if (numCpuCores <= 0)
            error("The parameter numCpuCores must be a positive value (>0)");

        if (maxUsers <= 0)
            error("The parameter maxUsers must be a positive value (>0)");
    }

    for (int i = 0; i < numCpuCores; i++)
        freeCoresArrayPtr[i] = true;

    //        // Create user vector
    //        userVector = new UserExecution* [maxVMs];
    //
    //        // Init user vector
    //        for (i=0; i<maxVMs; i++)
    //            userVector[i] = nullptr;
    //

    //        // Create aux structures for CPU schedulers
    //        if (isVirtualHardware){
    //
    //            cpuSchedulerUtilization = new bool[numCpuCores];
    //
    //            for (i=0; i<numCpuCores; i++)
    //               cpuSchedulerUtilization[i] = false;
    //    }
    //        else{
    //            cpuSchedulerUtilization = nullptr;
    //        }
}

double HardwareManager::getAvailableDisk() const
{
    return availableDisk;
}

void HardwareManager::setAvailableDisk(double availableDisk)
{
    this->availableDisk = availableDisk;
}

void HardwareManager::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

int HardwareManager::getAvailableRam()
{
    return availableMemory;
}

int HardwareManager::getAvailableCores()
{
    return numAvailableCpuCores;
}

int HardwareManager::getNumCores()
{
    return numCpuCores;
}

bool HardwareManager::allocateRam(double memory)
{
    if (memory > getAvailableRam())
        return false;

    availableMemory -= memory;

    return true;
}

bool HardwareManager::allocateDisk(double disk)
{
    if (disk > getAvailableDisk())
        return false;

    availableDisk -= disk;

    return true;
}

double HardwareManager::deallocateRam(double memory)
{
    availableMemory += memory;

    if (availableMemory > memorySize)
        availableMemory = memorySize;

    return availableMemory;
}

double HardwareManager::deallocateDisk(double disk)
{
    availableDisk += disk;

    if (availableDisk > diskSize)
        availableDisk = diskSize;

    return availableDisk;
}

unsigned int *HardwareManager::allocateCores(int numCores)
{
    if (numCores > getAvailableCores())
        return nullptr;

    int numAllocatedCores = 0;
    bool allocatedCore;
    unsigned int *cpuCoreIndex = new unsigned int[numCores];
    for (int i = 0; i < numCores; i++)
    {
        allocatedCore = false;
        for (int j = 0; j < numCpuCores && !allocatedCore; j++)
        {
            if (freeCoresArrayPtr[j])
            {
                cpuCoreIndex[i] = j;
                freeCoresArrayPtr[j] = false;
                numAllocatedCores++;
                allocatedCore = true;
            }
        }
    }

    if (numAllocatedCores < numCores)
    {
        for (int i = 0; i < numAllocatedCores; i++)
        {
            freeCoresArrayPtr[cpuCoreIndex[i]] = true;
        }
        cpuCoreIndex = nullptr;
    }
    else
    {
        int newNumAvailableCores = numAvailableCpuCores - numCores;
        numAvailableCpuCores = newNumAvailableCores > 0 ? newNumAvailableCores : 0;
    }

    return cpuCoreIndex;
}

int HardwareManager::deallocateCores(int numCores, unsigned int *cpuCoreIndex)
{
    for (int i = 0; i < numCores; i++)
    {
        freeCoresArrayPtr[cpuCoreIndex[i]] = true;
    }

    int newNumAvailableCores = numAvailableCpuCores + numCores;
    numAvailableCpuCores = newNumAvailableCores > numCpuCores ? numCpuCores : newNumAvailableCores;

    return numAvailableCpuCores;
}

bool *HardwareManager::getFreeCoresArrayPtr() const
{
    return freeCoresArrayPtr;
}
