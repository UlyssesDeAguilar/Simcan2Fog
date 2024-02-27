#include "../DataCentreManagerCost/DataCentreManagerCost.h"
#include "Management/utils/LogUtils.h"
Define_Module(DataCentreManagerCost);
using namespace hypervisor;

void DataCentreManagerCost::initialize()
{
    checkReservedFirst = par("checkReservedFirst");

    // Init super-class
    DataCentreManagerBase::initialize();
}

void DataCentreManagerCost::initializeRequestHandlers()
{
}

std::pair<uint32_t, size_t> DataCentreManagerCost::selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs)
{
    std::pair<uint32_t, size_t> selection;

    // Recover user and request info
    auto pCloudUser = check_and_cast<const CloudUserPriority *>(findUserTypeById(userVM_Rq->getUserId()));
    SM_UserVM_Cost *userVM_Rq_Cost = check_and_cast<SM_UserVM_Cost *>(userVM_Rq);

    if (checkReservedFirst && pCloudUser->getPriorityType() == Priority)
    {
        SM_UserVM_Cost *userVM_Rq_Cost = dynamic_cast<SM_UserVM_Cost *>(userVM_Rq);
        selection = selectNodeReserved(userVM_Rq_Cost, vmSpecs);
        
        // If there was an reserved node available, then return
        if (selection.first != UINT32_MAX)
        {
            userVM_Rq_Cost->setBPriorized(true);
            return selection;
        }
    }

    // Fallback or user is not of priority type
    selection = DataCentreManagerFirstFit::selectNode(userVM_Rq, vmSpecs);

    return selection;
}

std::pair<uint32_t, size_t> DataCentreManagerCost::selectNodeReserved(SM_UserVM_Cost *userVM_Rq, const VirtualMachine &vmSpecs)
{
    assert_msg((userVM_Rq != nullptr), "Nullpointer");

    std::string userId = userVM_Rq->getUserId();
    int numCoresRequested = vmSpecs.getNumCores();
    
    auto bucket = resourceManager->startFromCoreCount(numCoresRequested, true);
    auto end = resourceManager->endOfNodeMap(true);

    // Start checking
    for (; bucket != end; ++bucket)
    {
        auto vectorHypervisor = bucket->second;
        const DcResourceManager* rm = resourceManager;

        // Filter to get the first hypervisor which has at least the minimum needed
        auto filter = [vmSpecs, rm](const uint32_t &ip) -> bool
        { return rm->getNodeResources(ip) <= vmSpecs; };

        auto iter = std::find_if(vectorHypervisor.begin(), vectorHypervisor.end(), filter);
        
        // If there's a candidate, return immediately
        if (iter != vectorHypervisor.end())
            return std::pair<uint32_t, size_t>(*iter, iter - vectorHypervisor.begin());
    }

    // If not found
    return std::make_pair<>(UINT32_MAX, 0);
}
