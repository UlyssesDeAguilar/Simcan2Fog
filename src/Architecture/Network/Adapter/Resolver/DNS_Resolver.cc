#include "DNS_Resolver.h"

Define_Module(DNS_Resolver);

void DNS_Resolver::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        timeOut = par("timeOut");
    }
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
    responseSocket.setOutputGate(gate("socketOut"));
    responseSocket.bind(0);
    mainSocket.setCallback(this);
    responseSocket.setCallback(this);
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
    {
        handleTimeOut(msg);
        return;
    }
    // Multiplex incoming messages from SIMCAN / INET
    auto sm = dynamic_cast<SM_ResolverRequest *>(msg);

    if (sm != nullptr)
        handleResolution(sm);
    else if (mainSocket.belongsToSocket(msg))
        mainSocket.processMessage(msg);
    else if (responseSocket.belongsToSocket(msg))
        responseSocket.processMessage(msg);
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
        EV_INFO << "Requested url: " << url << " not cached" << endl;
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
        EV_INFO << "Requested NS/TLD: " << tld << " not cached" << endl;
        // Nothing found, so we really need to start from scratch
        if (newState->domain.isNameServer())
            newState->currentState = RequestState::URL_RESOLUTION;
        else
            newState->currentState = RequestState::TLD_RESOLUTION;
        dnsRequest->insertQuestion(tld.c_str());
        selectedIp = ROOT_DNS_IP;
    }

    EV_INFO << "Generated request with id: " << newId << " attempting resolution" << endl;

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
    while (pendingRequests.find(newId) != pendingRequests.end())
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
    EV_INFO << "Handling timeout for request: " << id << endl;

    // Search the request
    auto iter = pendingRequests.find(id);
    if (iter == pendingRequests.end())
        EV_ERROR << "Attempted to delete a non registered request -- ignored" << endl;
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
        EV_ERROR << "Recieved response for non registered request: " << requestId << " possible timeout" << endl;
        delete packet;
        return;
    }

    auto requestStatus = iter->second;

    if (response->getReturnCode() != ReturnCode::NOERROR || response->getRecordArraySize() != 1)
    {
        // DNS error -- delete and return failure
        EV_INFO << "Recieved error from DNS lookup for: " << requestId << " resolving: " << requestStatus->domain.getFullName() << endl;
        cancelAndDelete(requestStatus->timeOut);
        pendingRequests.erase(iter);
        sendResponse(requestStatus->request, nullptr);
        delete packet;
        return;
    }

    auto record = response->getRecord(0);
    if (requestStatus->currentState == RequestState::URL_RESOLUTION)
    {
        // Everything resolved
        EV_INFO << "Request with id: " << requestId << " completed succesfully" <<endl;
        cancelAndDelete(requestStatus->timeOut);
        pendingRequests.erase(iter);
        sendResponse(requestStatus->request, &record.ip);
    }
    else
    {
        // FIXME consider rechecking cache ?
        // Go from TLD_RESOLUTION to URL_RESOLUTION
        EV_INFO << "Request with id: " << requestId << " recieved NS/TLD, attempting final resolution" << endl;
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
    delete packet;
}

void DNS_Resolver::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    error("Error in socket communication");
}

void DNS_Resolver::socketClosed(UdpSocket *socket)
{
    // Technically it is not possible, so it is ignored
}
