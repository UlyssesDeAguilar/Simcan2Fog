#include "VmInstanceCollection.h"

VmInstanceCollection::VmInstanceCollection(VirtualMachine* vmPtr, std::string userID, int numInstances, int nRentTime, int total, int offset){

    // Check!
    if (vmPtr == nullptr)
        throw omnetpp::cRuntimeError("Constructor of VmInstanceCollection receives a null pointer to the base Application. #Instances:%d - User:%s", numInstances, userID.c_str());

    // VmInstance pointer to maintain general information of the VM
    vmBase = vmPtr;

    //Rentime initialization
    this->nRentTime = nRentTime;

    // Generate the replicas
    this->generateInstances(userID, numInstances, total, offset);
}

VmInstanceCollection::~VmInstanceCollection() {
    vmInstances.clear();
}

void VmInstanceCollection::generateInstances (std::string userID, int numInstances, int total, int offset){

    VmInstance *newInstance;
    int i;

    // Create and include replicas in the replica vector
    for (i=0; i < numInstances; i++)
      {
        newInstance = new VmInstance(vmBase->getType(), i + offset, total, userID);
        vmInstances.push_back(newInstance);
      }
}

VirtualMachine* VmInstanceCollection::getVirtualMachineBase (){
    return vmBase;
}

int VmInstanceCollection::getNumInstances (){
    return vmInstances.size();
}

std::string VmInstanceCollection::toString (bool includeVmFeatures){

    std::ostringstream info;
    int i;

        info << "# Instances:" << vmInstances.size() << " of " << vmBase->getType() << std::endl;

        for (i=0; i<vmInstances.size(); i++){
            info << "\t\t  - VMs[" << i << "]: " << vmInstances.at(i)->toString() << std::endl;
        }

        if (includeVmFeatures){
            info << "\t\t\t Features:" << vmBase->featuresToString() << std::endl;
        }

        info << std::endl;

    return info.str();
}
std::string VmInstanceCollection::getVmType()
{
    std::string strRet;

    strRet="";
    if(vmBase != nullptr)
    {
        strRet = vmBase->getType();
    }
    return strRet;
}
VmInstance* VmInstanceCollection::getVmInstance(int nIndex)
{
    VmInstance* pVmReturn;

    if(nIndex<vmInstances.size())
    {
        pVmReturn = vmInstances.at(nIndex);
    }

    return pVmReturn;
}
