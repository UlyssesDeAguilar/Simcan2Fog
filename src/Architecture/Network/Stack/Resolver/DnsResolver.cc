#include "DnsResolver.h"
#include "Applications/Base/UserAppBase/UserAppBase.h"

using namespace networkio;
using namespace dns;

Define_Module(DnsResolver);

void DnsResolver::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        ispResolver = L3Address(par("ispResolver"));
        multiplexer = check_and_cast<StackMultiplexer *>(getModuleByPath("^.sm"));
    }
    ApplicationBase::initialize(stage);
}

void DnsResolver::finish()
{
    // Release the socket (and the other resources)
    socket.close();
}

void DnsResolver::handleStartOperation(LifecycleOperation *operation)
{
    // Configure the socket
    socket.setOutputGate(gate("socketOut"));
    socket.bind(-1);
    socket.setCallback(this);
    // socket.connect(ispResolver, DNS_PORT);
}

void DnsResolver::handleStopOperation(LifecycleOperation *operation)
{
    error("Currently stopping is not supported");
}

void DnsResolver::handleCrashOperation(LifecycleOperation *operation)
{
    error("Currently stopping is not supported");
}

void DnsResolver::handleMessage(cMessage *msg)
{
    if (msg == timeOut)
    {
        // HACK then we'll get it ourselves
        auto dns = check_and_cast<DnsServiceSimplified*>(getModuleByPath("dns.dnsServer"));
        auto iter = pendingRequests.find(timeOut->getRequestId());

        if (iter == pendingRequests.end())
            error("Dns Resolver unexpected timeout");
        
        auto vec = dns->processQuestion(timeOut->getDomain());
        
        if (vec->size() == 0)
            error("Dns Resolver: no record found");
        
        auto &address = vec->at(0).ip;
        auto callback = check_and_cast<UserAppBase::ICallback *>(iter->second);
        callback->handleResolverReturned(address.toIpv4().getInt());

        pendingRequests.erase(iter);
        delete timeOut;
        timeOut = nullptr;
    }
    else
        ApplicationBase::handleMessage(msg);
}
void DnsResolver::handleMessageWhenUp(cMessage *msg)
{
    // Multiplex incoming messages from SIMCAN / INET
    auto packet = dynamic_cast<Packet *>(msg);
    if (packet)
        socket.processMessage(packet);
    else
    {
        processRequest(msg);
    }
}

void DnsResolver::processRequest(cMessage *msg)
{
    /* FIXME */
}

void DnsResolver::resolve(const char *domain, cModule *callback)
{
    Enter_Method_Silent();
    auto request = new DnsRequest();
    request->setOperationCode(QUERY);
    request->setRequestId(getNewRequestId());
    request->insertQuestion(domain);

    timeOut = new RequestTimeout();
    timeOut->setRequestId(request->getRequestId());
    timeOut->setDomain(domain);

    auto packet = new Packet("Dns Request", Ptr<DnsRequest>(request));
    socket.sendTo(packet, ispResolver, DNS_PORT);
    
    pendingRequests[request->getRequestId()] = callback;
    scheduleAt(simTime() + 10, timeOut);
}

uint16_t DnsResolver::getNewRequestId()
{
    if (lastId == UINT16_MAX)
        lastId = 0;
    return lastId++;
}

void DnsResolver::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    auto response = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
    auto iter = pendingRequests.find(response->getRequestId());

    if (iter == pendingRequests.end())
    {
        EV << "Ignoring not registered request with id: " << response->getRequestId() << "\n";
        delete packet;
        return;
    }

    auto callback = check_and_cast<UserAppBase::ICallback *>(iter->second);

    if (response->getReturnCode() == dns::ReturnCode::NOERROR)
    {
        const ResourceRecord &rec = response->getAuthoritativeAnswers(0);
        callback->handleResolverReturned(rec.ip.toIpv4().getInt());
    }
    else
    {
        callback->handleResolverReturned(0);
    }

    if (timeOut)
    {
        cancelAndDelete(timeOut);
        timeOut = nullptr;
    }
    
    // multiplexer->processResponse(event);
    pendingRequests.erase(iter);
    delete packet;
}

void DnsResolver::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    error("Error in socket communication");
}

void DnsResolver::socketClosed(UdpSocket *socket) {}
