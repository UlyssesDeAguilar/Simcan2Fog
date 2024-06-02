#include "HttpProxyService.h"
using namespace inet;

Define_Module(HttpProxyService);

TcpBaseProxyThread *HttpProxyService::newTcpThread() { return new HttpProxyThread(); }

void HttpProxyThread::selectFromPool(Ptr<const Chunk> chunk)
{
    auto httpRequest = dynamic_pointer_cast<const SM_REST_API>(chunk);

    auto serviceReference = proxy->getServicePoolMap().find(httpRequest->getUrl());

    if (serviceReference == proxy->getServicePoolMap().end())
        proxy->error("Tried to locate a service which isn't registered");

    auto &pool = serviceReference->second;
    auto find_if_available = [](const ServiceEntry &e)
    {
        return e.available;
    };

    // Find a free entry
    auto iter = std::find_if(pool.begin(), pool.end(), find_if_available);

    if (iter != pool.end())
    {
        // Reserve the node
        entry = const_cast<ServiceEntry *>(&*iter);
        entry->available = false;

        // Setup template message
        eventTemplate->setIp(entry->ip);
        eventTemplate->setVmId(entry->vmId);
        eventTemplate->setPid(entry->pid);
        eventTemplate->setSocketId(socket->getSocketId());
        eventTemplate->setKind(HTTP_PROXY);

        // Hold the service name
        service = &serviceReference->first;
    }
}