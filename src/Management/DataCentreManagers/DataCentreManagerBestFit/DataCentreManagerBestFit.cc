#include "DataCentreManagerBestFit.h"
Define_Module(DataCentreManagerBestFit);
using namespace hypervisor;

std::pair<uint32_t, size_t> DataCentreManagerBestFit::selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs)
{
    // Find the first node which has the minimum requirements for the VM
    auto currentNode = resourceManager->startFromCoreCount(vmSpecs.getNumCores());
    auto end = resourceManager->endOfNodeMap();

    // And then start to cycle (the next ones are guaranteed to have more cores than the one before)
    for (; currentNode != end; ++currentNode)
    {
        auto vectorHypervisor = currentNode->second;
        const DcResourceManager * rm = resourceManager;
        double minDistance = DBL_MAX;
        size_t selectedIndex = 0;

        // Select the one which has the least difference
        size_t i = 0;
        for (const auto &ip : vectorHypervisor)
        {
            double distance = rm->getNodeResources(ip).getLogicalDistance(vmSpecs);

            // If there's an exact match, return immediately
            if (distance == 0.0)
                return std::make_pair<>(ip, i);

            // Compare against the best match so far
            if (distance < minDistance)
            {
                selectedIndex = i;
                minDistance = distance;
            }
            i++;
        }

        // If the minDistance changed, we have a candidate
        if (minDistance < DBL_MAX)
            return std::make_pair<>(vectorHypervisor[selectedIndex], selectedIndex);
    }

    return std::make_pair<>(UINT32_MAX, 0);
}