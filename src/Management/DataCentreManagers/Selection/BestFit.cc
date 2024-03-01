#include "Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"
#include "Management/DataCentreManagers/ResourceManager/ResourceManager.h"
#include "Strategies.h"

using namespace dc;

uint32_t BestFit::selectNode(SM_UserVM *userVM_Rq, const VirtualMachine &vmSpecs)
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

        // Select the one which has the least difference
        size_t selectedIndex = 0;
        size_t i = 0;
        for (const auto &ip : vectorHypervisor)
        {
            double distance = rm->getNodeResources(ip).getLogicalDistance(vmSpecs);

            // If there's an exact match, return immediately
            if (distance == 0.0)
                return ip;

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
            return vectorHypervisor[selectedIndex];
    }

    return UINT32_MAX;
}