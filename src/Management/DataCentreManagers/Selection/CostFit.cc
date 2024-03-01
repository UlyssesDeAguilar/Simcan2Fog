#include "Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"
#include "Management/DataCentreManagers/ResourceManager/ResourceManager.h"
#include "Messages/SM_UserVM_Cost_m.h"
#include "Strategies.h"

using namespace dc;

uint32_t CostFit::selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs)
{
    // Recover user and request info
    auto pCloudUser = check_and_cast<const CloudUserPriority *>(manager->findUserTypeById(userVM_Rq->getUserId()));

    // If the user has priority
    if (pCloudUser->getPriorityType() == Priority)
    {
        SM_UserVM_Cost *userVM_Rq_Cost = check_and_cast<SM_UserVM_Cost *>(userVM_Rq);
        uint32_t selection = selectNode(userVM_Rq_Cost, vmSpecs);
        
        // If there was an reserved node available, then return
        if (selection != UINT32_MAX)
        {
            userVM_Rq_Cost->setBPriorized(true);
            return selection;
        }
    }

    // Fallback or the user was non priority
    return FirstFit::selectNode(userVM_Rq, vmSpecs);
}

uint32_t CostFit::selectNode(SM_UserVM_Cost *userVM_Rq, const VirtualMachine &vmSpecs)
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
            return *iter;
    }

    // If not found
    return UINT32_MAX;
}