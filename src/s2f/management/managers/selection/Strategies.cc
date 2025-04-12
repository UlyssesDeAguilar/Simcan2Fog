#include "s2f/management/managers/selection/Strategies.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <string_view>
#include <vector>

using namespace dc;
using namespace omnetpp;

Register_Abstract_Class(SelectionStrategy);
Register_Class(FirstFit);
Register_Class(BestFit);

bool FirstFit::selectNode(const VirtualMachine *vmSpecs, const std::vector<Node> &nodes, size_t &index)
{
    // Filter to get the first hypervisor which has at least the minimum needed
    auto filter = [vmSpecs](const Node &node) -> bool
    { return *vmSpecs <= node.availableResources; };

    auto iter = std::find_if(nodes.begin(), nodes.end(), filter);

    // Found the candidate
    if (iter != nodes.end())
    {
        index = std::distance(nodes.begin(), iter);
        return true;
    }

    // Not found
    return false;
}

struct NodeDistance
{
    double distance;
    size_t index;
};

bool operator>(const NodeDistance &lhs, const NodeDistance &rhs) { return lhs.distance > rhs.distance; }
bool operator<(const NodeDistance &lhs, const NodeDistance &rhs) { return lhs.distance < rhs.distance; }

bool BestFit::selectNode(const VirtualMachine *vmSpecs, const std::vector<Node> &nodes, size_t &index)
{
    std::vector<NodeDistance> distances;
    distances.reserve(nodes.size());

    // Min heap O(lg(N))
    for (size_t i = 0; i < nodes.size(); i++)
    {
        distances.push_back({nodes[i].availableResources.getLogicalDistance(*vmSpecs), i});
        std::make_heap(distances.begin(), distances.end(), std::greater<NodeDistance>());
        
        // Up to debate if this accelerates or not, largely depends on N
        // If it's an exact match, return immediately
        if (distances.front().distance == 0.0)
        {
            index = distances.front().index;
            return true;
        }
    }

    auto &bestNode = distances.front();
    if (bestNode.distance == DBL_MAX)
        return false;
    
    index = bestNode.index;
    return true;
}


/**
 * Note on the following section
 * This is a deprecated implementation, it should be done at the ResourceManager by selecting a "reserved node" pool.
 * After all, it's just first fit over a pool
 */

/*
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
    auto bucket = resourceManager->startFromCoreCount(vmSpecs.getNumCores(), true);
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
}*/