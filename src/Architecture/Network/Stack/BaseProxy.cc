#include "BaseProxy.h"

using namespace omnetpp;
using namespace dns;

void BaseProxy::initialize(int stage)
{
    if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        // Locate the multiplexer
        multiplexer = check_and_cast<StackMultiplexer *>(getModuleByPath("^.sm"));
        // Locate the local dns server
        localDns = check_and_cast<DnsService *>(getModuleByPath("^.dnsServer"));
    }

    ApplicationBase::initialize(stage);
}

void BaseProxy::processRequest(omnetpp::cMessage *msg)
{
    auto command = check_and_cast<networkio::CommandEvent *>(msg);

    switch (command->getCommand())
    {
    case networkio::SOCKET_OPEN:
    {
        ServiceEntry newEntry;
        newEntry.pid = command->getPid();
        newEntry.vmId = command->getVmId();
        newEntry.ip = command->getIp();

        auto &servicePool = findOrRegisterService(command->getServiceName(), command->getTargetPort());
        servicePool.insert(newEntry);
        break;
    }
    case networkio::SOCKET_CLOSE:
    {
        std::string serviceName(command->getServiceName());
        ServiceEntry oldEntry;
        oldEntry.pid = command->getPid();
        oldEntry.vmId = command->getVmId();
        oldEntry.ip = command->getIp();

        removeFromPool(command->getServiceName(), oldEntry);
        break;
    }
    default:
        break;
    }
}

std::set<ServiceEntry> &BaseProxy::findOrRegisterService(const char *service, const uint32_t serviceIp)
{
    // Attempt to find previously registered service
    auto iter = servicePoolMap.find(service);
    if (iter != servicePoolMap.end())
    {
        return iter->second;
    }
    else
    {
        localDns->registerService(service);
        return servicePoolMap[service];
    }
}

void BaseProxy::removeFromPool(const char *service, const ServiceEntry &oldEntry)
{
    // Attempt to find previously registered service
    auto iter = servicePoolMap.find(service);
    if (iter != servicePoolMap.end())
    {
        // Reference to the ServicePool
        auto pool = iter->second;

        // Erase the old entry
        pool.erase(oldEntry);

        if (pool.size() == 0)
            // Register the record
            localDns->unregisterService(service);
    }
    else
        error("Attempted to remove a ServiceEntry for a service which is not registered");
}