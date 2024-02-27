#include "../DataCentreManagerFirstFit/DataCentreManagerFirstFit.h"
#include "Management/utils/LogUtils.h"

using namespace hypervisor;

Define_Module(DataCentreManagerFirstFit);

std::pair<uint32_t, size_t> DataCentreManagerFirstFit::selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs)
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
            return std::pair<uint32_t, size_t>(*iter, iter - vectorHypervisor.begin());
    }

    return std::make_pair<>(UINT32_MAX, 0);
}
