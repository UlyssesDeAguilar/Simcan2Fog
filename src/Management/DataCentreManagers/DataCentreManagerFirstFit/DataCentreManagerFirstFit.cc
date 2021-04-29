#include "../DataCentreManagerFirstFit/DataCentreManagerFirstFit.h"

#include "Management/utils/LogUtils.h"


Define_Module(DataCentreManagerFirstFit);

DataCentreManagerFirstFit::~DataCentreManagerFirstFit(){
}

void DataCentreManagerFirstFit::initialize(){

    // Init super-class
    DataCentreManagerBase::initialize();
}

Hypervisor* DataCentreManagerFirstFit::selectNode (SM_UserVM*& userVM_Rq, const VM_Request& vmRequest){
    VirtualMachine *pVMBase;
    Hypervisor *pHypervisor = nullptr;
    NodeResourceRequest *pResourceRequest;
    int numCoresRequested, numNodeTotalCores, numAvailableCores;
    std::map<int, std::vector<Hypervisor*>>::iterator itMap;
    std::vector<Hypervisor*> vectorHypervisor;
    std::vector<Hypervisor*>::iterator itVector;
    bool bHandled;
    string strUserName;

    if (userVM_Rq==nullptr) return nullptr;

    strUserName = userVM_Rq->getUserID();

    pVMBase = findVirtualMachine(vmRequest.strVmType);
    numCoresRequested = pVMBase->getNumCores();

    bHandled = false;
    for (itMap = mapHypervisorPerNodes.begin(); itMap != mapHypervisorPerNodes.end() && !bHandled; ++itMap){
        numNodeTotalCores = itMap->first;
        if (numNodeTotalCores >= numCoresRequested) {
            vectorHypervisor = itMap->second;
            for (itVector = vectorHypervisor.begin(); itVector != vectorHypervisor.end() && !bHandled; ++itVector) {
                pHypervisor = *itVector;
                numAvailableCores = pHypervisor->getAvailableCores();
                if (numAvailableCores >= numCoresRequested) {
                    pResourceRequest = generateNode(strUserName, vmRequest);
                    bHandled = allocateVM(pResourceRequest, pHypervisor);
                    if (bHandled) {
                        manageActiveMachines();
                        return pHypervisor;
                    }
                }
            }

        }
    }

    return nullptr;
}
