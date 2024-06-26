#include "../../../management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"
#include "../../../management/DataCentreManagers/ResourceManager/ResourceManager.h"
#include "../../../management/DataCentreManagers/Selection/Strategies.h"

using namespace dc;

uint32_t FirstFit::selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs)
{
    std::string userId = userVM_Rq->getUserId();

    // Find the first node which has the minimum requirements for the VM
    auto currentNode = resourceManager->startFromCoreCount(vmSpecs.getNumCores());
    auto end = resourceManager->endOfNodeMap();
    const DcResourceManager* rm = resourceManager;

    // And then start to cycle (the next ones are guaranteed to have more cores than the one before)
    for (; currentNode != end; ++currentNode)
    {
        auto vectorHypervisor = currentNode->second;

        // Filter to get the first hypervisor which has at least the minimum needed
        auto filter = [vmSpecs, rm](const uint32_t &ip) -> bool
        { return rm->getNodeResources(ip) <= vmSpecs; };

        auto iter = std::find_if(vectorHypervisor.begin(), vectorHypervisor.end(), filter);
        
        // If there's a candidate, return immediately
        if (iter != vectorHypervisor.end())
            return *iter;
    }

    return UINT32_MAX;
}