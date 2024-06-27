#include "ResourceManager.h"

void VmDeployment::addNode(const uint32_t &nodeIp, VM_Request &request, const VirtualMachine *vm)
{
    // Update node
    resourceManager->updateNode(nodeIp, vm, true);

    // Register allocation
    allocations.emplace_back(nodeIp, request, vm);
}

void VmDeployment::rollback()
{
    for (const auto &allocation : allocations)
        resourceManager->updateNode(allocation.nodeIp, allocation.vm, false);

    // After rolling back, clear all changes
    allocations.clear();
}

void VmDeployment::commit()
{
    ServiceURL templateUrl(0, resourceManager->globalAddress);

    for (const auto &allocation : allocations)
    {
        // Prepare ServiceURL in string format
        std::ostringstream stream;
        templateUrl.setLocalAddress(allocation.nodeIp);
        stream << templateUrl;

        // Recover vm request and fill in the data
        VM_Response &response = allocation.request.response;
        response.state = VM_Response::ACCEPTED;
        response.startTime = simTime().dbl();
        response.ip = stream.str();
        response.price = 0; // FIXME: This is what the "original" code in DcManager did, ¿what's the true meaning of this field?

        // Notify the resource manager the effectiveness of the changes
        resourceManager->confirmNodeAllocation(allocation.nodeIp, allocation.vm);

        // Really allocate the vm, topologically speaking
        auto hypervisor = resourceManager->getHypervisor(allocation.nodeIp);
        hypervisor->handleVmRequest(allocation.request, vmsRequest->getUserId());
    }

    // Clear all allocations
    allocations.clear();
}