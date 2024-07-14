#include "DnsResolver.h"
#include "s2f/apps/models/AppBase.h"

using namespace dns;

Define_Module(DnsResolver);

void DnsResolver::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        ispResolver = L3Address(par("ispResolver"));
    }
    ApplicationBase::initialize(stage);
}

void DnsResolver::finish()
{
    for (auto iter : pendingRequests)
        freeContext(iter.second);
    
    pendingRequests.clear();
}

void DnsResolver::handleStartOperation(LifecycleOperation *operation)
{
    // Configure the socket
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
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
    auto timeOut = dynamic_cast<RequestTimeout *>(msg);
    if (timeOut)
    {
        auto &ctx = pendingRequests[timeOut->getRequestId()];
        ctx.remainingAttempts--;

        if (ctx.remainingAttempts <= 0)
        {
            L3Address address;
            invokeCallback(timeOut->getRequestId(), address, false);
            return;
        }

        socket.sendTo(ctx.packet->dup(), ispResolver, DNS_PORT);
        scheduleAt(simTime() + par("requestTimeout"), timeOut);
    }
    else
        ApplicationBase::handleMessage(msg);
}

void DnsResolver::handleMessageWhenUp(cMessage *msg)
{
    socket.processMessage(check_and_cast<Packet *>(msg));
}

void DnsResolver::resolve(const char *domain, ResolverCallback *callback)
{
    Enter_Method("Resolving domain: %s\n", domain);
    RequestContext ctx;

    const auto &request = makeShared<DnsRequest>();
    request->setOperationCode(QUERY);
    request->setRequestId(getNewRequestId());
    request->insertQuestion(domain);

    auto packet = new Packet("Dns Request");
    packet->insertAtBack(request);

    auto timeOut = new RequestTimeout();
    timeOut->setRequestId(request->getRequestId());
    timeOut->setDomain(domain);

    ctx.remainingAttempts = par("retryCount");
    ctx.callback = callback;
    ctx.timeOut = timeOut;
    ctx.packet = packet;

    pendingRequests[request->getRequestId()] = ctx;

    socket.sendTo(packet->dup(), ispResolver, DNS_PORT);
    scheduleAt(simTime() + par("requestTimeout"), timeOut);
}

void DnsResolver::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    auto response = dynamic_pointer_cast<const DnsRequest>(packet->peekData());
    L3Address address;
    bool resolved;

    if (response->getReturnCode() == dns::ReturnCode::NOERROR)
    {
        const ResourceRecord &rec = response->getAuthoritativeAnswers(0);
        address = rec.ip;
        resolved = true;
    }
    else
    {
        resolved = false;
    }

    invokeCallback(response->getRequestId(), address, resolved);
    delete packet;
}

void DnsResolver::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    delete indication;
    error("Error in socket communication");
}

void DnsResolver::socketClosed(UdpSocket *socket) {}

void DnsResolver::invokeCallback(uint16_t requestId, const L3Address &address, bool resolved)
{
    auto iter = pendingRequests.find(requestId);
    if (iter == pendingRequests.end())
    {
        EV_WARN << "Ignoring not registered request with id: " << requestId << "\n";
        return;
    }

    // Invoke the callback
    auto &ctx = iter->second;
    ctx.callback->handleResolverReturned(address, resolved);

    // Erase the context
    freeContext(ctx);
    pendingRequests.erase(iter);
}

uint16_t DnsResolver::getNewRequestId()
{
    if (lastId == UINT16_MAX)
        lastId = 0;
    return lastId++;
}

void DnsResolver::freeContext(RequestContext &ctx)
{
    delete ctx.packet;
    cancelAndDelete(ctx.timeOut);
}