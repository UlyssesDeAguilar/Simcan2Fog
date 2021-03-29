#include "DataCenterManagerBestFit.h"

#include "Management/utils/LogUtils.h"


Define_Module(DataCenterManagerBestFit);

DataCenterManagerBestFit::~DataCenterManagerBestFit(){
}

void DataCenterManagerBestFit::initialize(){

    // Init super-class
    DataCenterManager::initialize();
}

Hypervisor* DataCenterManagerBestFit::selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest){
    VirtualMachine *pVMBase;
    Hypervisor *pHypervisor = nullptr;
    NodeResourceRequest *pResourceRequest;
    int numCoresRequested, numAvailableCores;
    std::map<int, std::vector<Hypervisor*>>::iterator itMap;
    std::vector<Hypervisor*>::iterator itVector;
    bool bHandled;
    string strUserName;

    if (userVM_Rq==nullptr) return nullptr;

    strUserName = userVM_Rq->getUserID();

    pVMBase = findVirtualMachine(vmRequest.strVmType);
    numCoresRequested = pVMBase->getNumCores();

    bHandled = false;
    for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end() && !bHandled; ++itMap){
        numAvailableCores = itMap->first;
        if (numAvailableCores >= numCoresRequested) {
            std::vector<Hypervisor*> &vectorHypervisor = itMap->second;
            for (itVector = vectorHypervisor.begin(); itVector != vectorHypervisor.end() && !bHandled; ++itVector) {
                pHypervisor = *itVector;
                numAvailableCores = pHypervisor->getAvailableCores();
                if (numAvailableCores >= numCoresRequested) {
                    pResourceRequest = generateNode(strUserName, vmRequest);
                    bHandled = allocateVM(pResourceRequest, pHypervisor);

                    if (bHandled) {
                        // Move to right vector by avaible cores
                        vectorHypervisor.erase(itVector);
                        mapHypervisorPerNodes[pHypervisor->getAvailableCores()].push_back(pHypervisor);

                        return pHypervisor;
                    }
                }
            }

        }
    }

    return nullptr;
}

void DataCenterManagerBestFit::deallocateVmResources(std::string strVmId)
{
    std::map<int, std::vector<Hypervisor*>>::iterator itMap;
    std::vector<Hypervisor*> vectorHypervisor;
    Hypervisor *pHypervisor = getNodeHypervisorByVm(strVmId);

    if (pHypervisor==nullptr)
        error ("%s - Unable to deallocate VM. Wrong VM name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strVmId.c_str());

    removeNodeFromMap(pHypervisor);

    pHypervisor->deallocateVmResources(strVmId);

    storeNodeInMap(pHypervisor);

    updateCpuUtilizationTimeForHypervisor(pHypervisor);
}

void DataCenterManagerBestFit::storeNodeInMap (Hypervisor* pHypervisor){
    if (pHypervisor!=nullptr) {
        mapHypervisorPerNodes[pHypervisor->getAvailableCores()].push_back(pHypervisor);
    }

}

void DataCenterManagerBestFit::removeNodeFromMap (Hypervisor* pHypervisor){
    std::map<int, std::vector<Hypervisor*>>::iterator itMap;

    if (pHypervisor!=nullptr) {
        itMap = mapHypervisorPerNodes.find(pHypervisor->getAvailableCores());

        if (itMap != mapHypervisorPerNodes.end()) {
            std::vector<Hypervisor*> &vectorHypervisor = itMap->second;
            vectorHypervisor.erase(std::remove(vectorHypervisor.begin(), vectorHypervisor.end(), pHypervisor), vectorHypervisor.end());
        }
    }
}
