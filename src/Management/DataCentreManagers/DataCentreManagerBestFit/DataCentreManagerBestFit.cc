#include "DataCentreManagerBestFit.h"
Define_Module(DataCentreManagerBestFit);
using namespace hypervisor;

void DataCentreManagerBestFit::initialize()
{
    // Init super-class
    DataCentreManagerBase::initialize();
}

DcHypervisor *DataCentreManagerBestFit::selectNode(SM_UserVM *&userVM_Rq, const VM_Request &vmRequest)
{
    if (userVM_Rq == nullptr)
        return nullptr;

    auto userId = userVM_Rq->getUserId();
    auto pVMBase = dataManager->searchVirtualMachine(vmRequest.vmType);
    int numCoresRequested = pVMBase->getNumCores();

    // Search in buckets the available nodes with the needed cores
    for (const auto &entry : mapHypervisorPerNodes)
    {
        int numAvailableCores = entry.first;

        // If we don't have enough resources go to the next node
        if (numAvailableCores < numCoresRequested)
            continue;

        // Search in buckets the available Hypervisors with the needed cores
        auto vectorHypervisor = entry.second;
        int i = 0;
        for (const auto &hypervisor : vectorHypervisor)
        {
            numAvailableCores = hypervisor->getAvailableCores();

            // If we don't have enough resources go to the next hypervisor
            if (numAvailableCores < numCoresRequested)
            {
                i++;
                continue;
            }

            if (allocateVM(vmRequest, hypervisor))
            {
                // As order doesn't matter, swap with last element and then pop back
                // This is O(1) vs std::vector::erase which is O(N)
                std::swap(vectorHypervisor[i], vectorHypervisor.back());
                vectorHypervisor.pop_back();

                // Move to right vector by avaible cores
                mapHypervisorPerNodes[hypervisor->getAvailableCores()].push_back(hypervisor);
                manageActiveMachines();

                return hypervisor;
            }
            else
            {
                // In case the allocation fails, release resources and return
                return nullptr;
            }
        }
    }

    // There wasn't any available node with the necessary resources
    return nullptr;
}

void DataCentreManagerBestFit::deallocateVmResources(std::string strVmId)
{
    DcHypervisor *pHypervisor = getNodeHypervisorByVm(strVmId);

    if (pHypervisor == nullptr)
        error("%s - Unable to deallocate VM. Wrong VM name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), strVmId.c_str());

    removeNodeFromMap(pHypervisor);

    pHypervisor->deallocateVmResources(strVmId);

    storeNodeInMap(pHypervisor);

    updateCpuUtilizationTimeForHypervisor(pHypervisor);
    manageActiveMachines();
}

void DataCentreManagerBestFit::storeNodeInMap(DcHypervisor *pHypervisor)
{
    if (pHypervisor != nullptr)
    {
        mapHypervisorPerNodes[pHypervisor->getAvailableCores()].push_back(pHypervisor);
    }
}

void DataCentreManagerBestFit::removeNodeFromMap(DcHypervisor *pHypervisor)
{
    std::map<int, std::vector<DcHypervisor *>>::iterator itMap;

    if (pHypervisor != nullptr)
    {
        itMap = mapHypervisorPerNodes.find(pHypervisor->getAvailableCores());

        if (itMap != mapHypervisorPerNodes.end())
        {
            std::vector<DcHypervisor *> &vectorHypervisor = itMap->second;
            vectorHypervisor.erase(std::remove(vectorHypervisor.begin(), vectorHypervisor.end(), pHypervisor), vectorHypervisor.end());
        }
    }
}
