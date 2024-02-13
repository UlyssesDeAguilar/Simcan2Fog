#include "../DataCentreManagerFirstFit/DataCentreManagerFirstFit.h"
#include "Management/utils/LogUtils.h"

using namespace hypervisor;

Define_Module(DataCentreManagerFirstFit);

void DataCentreManagerFirstFit::initialize()
{
    // Init super-class
    DataCentreManagerBase::initialize();
}


DcHypervisor *DataCentreManagerFirstFit::selectNode(SM_UserVM *&userVM_Rq, const VM_Request &vmRequest)
{
    if (userVM_Rq == nullptr)
        return nullptr;

    std::string userId = userVM_Rq->getUserId();

    auto pVMBase = dataManager->searchVirtualMachine(vmRequest.vmType);
    int numCoresRequested = pVMBase->getNumCores();

    // Find the first node which has the minimum requirements for the VM
    auto currentNode = resourceManager->startFromCoreCount(numCoresRequested);
    auto end = resourceManager->endOfNodeMap();

    // And then start to cycle (the next ones are guaranteed to have more cores than the one before)
    for (; currentNode != end; ++currentNode)
    {
        auto vectorHypervisor = currentNode->second;

        // Filter to get the first hypervisor which has the minimum needed
        auto filter = [numCoresRequested](DcHypervisor* &h) -> bool
        { return h->getAvailableCores() >= numCoresRequested; };

        auto hypervisor = std::find_if(vectorHypervisor.begin(), vectorHypervisor.end(), filter);
        
        // If there's a candidate
        if (hypervisor != vectorHypervisor.end())
        {
            if (allocateVM(vmRequest, *hypervisor))
            {
                manageActiveMachines();
                return *hypervisor;
            }
            else
            {
                // Release the generated node request
                return nullptr;
            }
        }
    }

    return nullptr;
}
