#include "DNS_Resolver.h"

Define_Module(DNS_Resolver);

void DNS_Resolver::initialize(int stage)
{
    // Connect to the dispatcher ?
    ApplicationBase::initialize(stage);
}

void DNS_Resolver::finish()
{
    // Release the socket (and the other resources)
    mainSocket.close();
}

void DNS_Resolver::handleStartOperation(LifecycleOperation *operation)
{
    // Configure the socket
    mainSocket.setOutputGate(gate("socketOut"));
    mainSocket.setCallback(this);
}

void DNS_Resolver::handleStopOperation(LifecycleOperation *operation)
{
    error("Currently stopping is not supported");
}
void DNS_Resolver::handleCrashOperation(LifecycleOperation *operation)
{
    error("Currently stopping is not supported");
}

void DNS_Resolver::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage())
        handleTimeOut(msg);

    // Multiplex incoming messages from SIMCAN / INET
    auto sm = dynamic_cast<SM_ResolverRequest *>(msg);

    if (sm != nullptr)
        handleResolution(sm);
    else if (mainSocket.belongsToSocket(msg))
        mainSocket.processMessage(msg);
    else
        error("Recieved unkown message");
}

void DNS_Resolver::sendResponse(SM_ResolverRequest *request, L3Address *ip)
{
    // Set the flag to return
    request->setIsResponse(true);

    // Prepare the fields for OK/ERROR response
    if (ip == nullptr)
        request->setResult(SC_ERROR);
    else
    {
        request->setResult(SC_OK);
        request->setResolvedIp(*ip);
    }

    // Send to the RequestMultiplexer
    send(request, gate("moduleOut"));
}

void DNS_Resolver::handleResolution(SM_ResolverRequest *request)
{
    auto url = std::string(request->getDomainName());

    try
    {
        auto record = recordCache.at(url);
        sendResponse(request, &record.ip);
    }
    catch (std::out_of_range const &e)
    {
        EV_DEBUG << "Requested url: " << url << " not cached" << endl;
    }

    // We will need to ask the servers --> init the request state
    auto newId = getNewRequestId();
    auto newState = new RequestState(url, request);

    // Prepare the request
    auto dnsRequest = new DNS_Request();
    dnsRequest->setRequestId(newId);
    dnsRequest->setOperationCode(OP_Code::QUERY);

    std::string tld;
    L3Address selectedIp;

    try
    {
        newState->domain.getTopLevelDomainName(tld);
        auto record = recordCache.at(tld);
        // If found the TLD/NS -- ask for the url
        dnsRequest->insertQuestion(url.c_str());
        selectedIp = record.ip;
    }
    catch (std::out_of_range const &e)
    {
        EV_DEBUG << "Requested NS/TLD: " << tld << " not cached" << endl;
        // Nothing found, so we really need to start from scratch
        newState->currentState = RequestState::TLD_RESOLUTION;
        dnsRequest->insertQuestion(tld.c_str());
        selectedIp = ROOT_DNS_IP;
    }

    // Compose and send packet
    auto packet = new Packet("DNS_Request", Ptr<DNS_Request>(dnsRequest));
    mainSocket.sendTo(packet, selectedIp, DNS_PORT);

    // Prepare the timeout and register the request
    newState->timeOut = prepareRequestTimeOut(newId);
    pendingRequests[newId] = newState;
}

uint16_t DNS_Resolver::getNewRequestId()
{
    // FIXME: Macro for the range of uint16_t ?
    uint16_t newId = (lastId + 1) % 65536;

    // Find the next non occupied id
    while (pendingRequests.find(newId) == pendingRequests.end())
        newId++;

    // Store the last assigned id
    lastId = newId;

    return newId;
}

cMessage *DNS_Resolver::prepareRequestTimeOut(uint16_t requestId)
{
    auto timeOutEvent = new cMessage("Resolution TIMEOUT", requestId);
    scheduleAt(simTime() + timeOut, timeOutEvent);
    return timeOutEvent;
}

void DNS_Resolver::handleTimeOut(cMessage *msg)
{
    uint16_t id = msg->getKind();
    
    // Search the request
    auto iter = pendingRequests.find(id);
    if (iter == pendingRequests.end())
        error("Recieved timeout for non existing request!");
    else
    {
        auto pendingRequest = iter->second;
        pendingRequests.erase(iter);

        // Extract SM_ResolverRequest and erase the associated state!
        auto originalRequest = pendingRequest->request;
        delete pendingRequest;

        // Send the error
        sendResponse(originalRequest, nullptr);
    }

    // Delete the event
    delete msg;
}

void DNS_Resolver::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // DNS Response analysis
    auto response = dynamic_cast<const DNS_Request *>(packet->peekData().get());
    if (response == nullptr)
        error("Non DNS response recieved via UDP");

    // Retrieve Id and then the status
    uint16_t requestId = response->getRequestId();
    auto iter = pendingRequests.find(requestId);

    if (iter == pendingRequests.end())
    {
        EV << "Recieved response for request: " << response->getRequestId() << " which probably recieved timeout." << endl;
        return;
    }

    auto requestStatus = iter->second;

    // FIXME -- Behavior of DNS_Service should indicate in response an error
    if (response->getReturnCode() == ReturnCode::NOERROR && response->getRecordArraySize() == 1)
    {
        auto record = response->getRecord(0);
        if (requestStatus->currentState == RequestState::URL_RESOLUTION)
        {
            // Everything resolved
            cancelAndDelete(requestStatus->timeOut);
            pendingRequests.erase(iter);
            sendResponse(requestStatus->request, &record.ip);
        }
        else
        {
            // Go from TLD_RESOLUTION to URL_RESOLUTION
            requestStatus->currentState = RequestState::URL_RESOLUTION;

            // Prepare the query
            auto dnsRequest = new DNS_Request();
            dnsRequest->setOperationCode(OP_Code::QUERY);
            dnsRequest->insertQuestion(requestStatus->domain.getFullName().c_str());

            // Prepare package and send it to the NS/TLD
            auto packet = new Packet("DNS_Request", Ptr<DNS_Request>(dnsRequest));
            mainSocket.sendTo(packet, record.ip, DNS_PORT);
        }

        // In both cases, cache the returned info
        recordCache[record.url] = record;
    }
    else
    {
        // DNS error -- delete and return failure
        cancelAndDelete(requestStatus->timeOut);
        pendingRequests.erase(iter);
        sendResponse(requestStatus->request, nullptr);
    }
}

void DNS_Resolver::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    error("Error in socket communication");
}

void DNS_Resolver::socketClosed(UdpSocket *socket)
{
    // Technically it is not possible, so it is ignored
}
