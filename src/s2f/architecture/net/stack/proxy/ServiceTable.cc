#include <algorithm>
#include "s2f/architecture/net/stack/proxy/ServiceTable.h"

using namespace omnetpp;

Define_Module(ServiceTable);

void ServiceTable::initialize()
{
    domain = par("domain");
    EV_INFO << "Initializing service table for domain " << domain << "\n";
}

void ServiceTable::finish()
{
    // Clear old entries
    serviceMap.clear();
    subscribers.clear();
}

void ServiceTable::registerService(const char *serviceName, int ip, int port)
{
    Enter_Method_Silent("Service registration for service %s\n", serviceName);
    std::vector<ServiceEntry> &entries = serviceMap[serviceName];
    entries.push_back(ServiceEntry(ip, port));

    if (entries.size() == 1)
    {
        EV_INFO << "Service registered: " << serviceName << " on domain " << domain << "\n";
        for (auto subscriber : subscribers)
            subscriber->serviceRegistered(serviceName);
    }
}

void ServiceTable::unregisterService(const char *serviceName, int ip, int port)
{
    Enter_Method_Silent("Service unregistration for service %s\n", serviceName);
    std::vector<ServiceEntry> &entries = serviceMap[serviceName];
    ServiceEntry::Id key = {.address = {static_cast<uint32_t>(ip), static_cast<uint32_t>(port)}};

    auto iter = std::find_if(entries.begin(), entries.end(), [key](const ServiceEntry &entry)
                             { return entry.entry.id == key.id; });

    std::swap(*iter, entries.back());
    entries.pop_back();

    if (entries.empty())
    {
        EV_INFO << "Service unregistered: " << serviceName << " on domain " << domain << "\n";
        for (auto subscriber : subscribers)
            subscriber->serviceUnregistered(serviceName);
    }
}
