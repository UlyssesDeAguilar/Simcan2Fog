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
        localDns = check_and_cast<DNS_Service *>(getModuleByPath("^.app[3]"));
    }

    ApplicationBase::initialize(stage);
}

void BaseProxy::processRequest(omnetpp::cMessage *msg)
{
    auto command = check_and_cast<networkio::Event *>(msg);

    switch (command->getCommand())
    {
    case networkio::SOCKET_OPEN:
    {
        std::string serviceName(command->getServiceName());
        ServiceEntry newEntry;
        newEntry.pid = command->getPid();
        newEntry.vmId = command->getVmId();
        newEntry.ip = command->getIp();

        auto &servicePool = findOrRegisterService(serviceName, command->getRips());
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

        removeFromPool(serviceName, oldEntry);
        break;
    }
    default:
        break;
    }
}

std::set<ServiceEntry> &BaseProxy::findOrRegisterService(const std::string &service, const uint32_t serviceIp)
{
    // Attempt to find previously registered service
    auto iter = servicePoolMap.find(service);
    if (iter != servicePoolMap.end())
    {
        return iter->second;
    }
    else
    {
        dns::ResourceRecord record;

        // Prepare the register
        record.ip = Ipv4Address(serviceIp);
        record.type = dns::RR_Type::A;
        record.url = service;

        // Register the record
        dnsTemplate.insertRecord(record);
        localDns->handleInsert(&dnsTemplate, &dnsTemplate);
        dnsTemplate.eraseRecord(0);

        return servicePoolMap[service];
    }
}

void BaseProxy::removeFromPool(const std::string &service, const ServiceEntry &oldEntry)
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
        {
            // Prepare record, only url is needed for deletion
            dns::ResourceRecord record;
            record.url = service;

            // Register the record
            dnsTemplate.insertRecord(record);
            localDns->handleDelete(&dnsTemplate, &dnsTemplate);
            dnsTemplate.eraseRecord(0);
        }
    }
    else
        error("Attempted to remove a ServiceEntry for a service which is not registered");
}