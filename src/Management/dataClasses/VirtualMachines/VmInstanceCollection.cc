#include "VmInstanceCollection.h"

VmInstanceCollection::VmInstanceCollection(const VirtualMachine *vmPtr, std::string userID, int numInstances, int nRentTime, int total, int offset)
{
    // Check!
    if (vmPtr == nullptr)
        throw omnetpp::cRuntimeError("Constructor of VmInstanceCollection receives a null pointer to the base Application. #Instances:%d - User:%s", numInstances, userID.c_str());

    // VmInstance pointer to maintain general information of the VM
    vmBase = vmPtr;

    // Rentime initialization
    this->nRentTime = nRentTime;

    // Generate the replicas
    this->generateInstances(userID, numInstances, total, offset);
}

VmInstanceCollection::~VmInstanceCollection()
{
    vmInstances.clear();
}

void VmInstanceCollection::generateInstances(std::string userID, int numInstances, int total, int offset)
{
    // Create and include replicas in the replica vector
    for (int i = 0; i < numInstances; i++)
    {
        auto newInstance = new VmInstance(vmBase->getType(), i + offset, total, userID);
        vmInstances.push_back(newInstance);
    }
}

std::string VmInstanceCollection::toString(bool includeVmFeatures)
{
    std::ostringstream info;

    info << "# Instances:" << vmInstances.size() << " of " << vmBase->getType() << "\n";

    int i = 0;
    for (auto const& instance: vmInstances)
        info << "\t\t  - VMs[" << i++ << "]: " << instance->toString() << "\n";

    if (includeVmFeatures)
        info << "\t\t\t Features:" << *vmBase << "\n";

    info << "\n";

    return info.str();
}
