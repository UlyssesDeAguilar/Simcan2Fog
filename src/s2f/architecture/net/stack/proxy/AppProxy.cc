#include "AppProxy.h"

#include "inet/common/socket/SocketTag_m.h"
#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

using namespace inet;
using namespace omnetpp;

Define_Module(AppProxy);

void AppProxy::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        inGateBase = gate("appIn", 0)->getBaseId();
        outGateBase = gate("appOut", 0)->getBaseId();
        transportGateIn = gate("transportIn")->getBaseId();
        transportGateOut = gate("transportOut")->getBaseId();
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {

        serverSocket.setOutputGate(gate(transportGateOut));
        serverSocket.bind(par("localPort"));
        serverSocket.listen();

        serviceTable = check_and_cast<ServiceTable *>(findModuleByPath(par("serviceTablePath")));

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void AppProxy::finish()
{
    socketToConnnectionMap.clear();
    socketToSessionMap.clear();
}

void AppProxy::handleMessage(cMessage *msg)
{
    if (msg->getArrivalGate()->getId() == transportGateIn)
        handleLowerMessage(msg);
    else
        handleUpperMessage(msg);
}

void AppProxy::handleUpperMessage(cMessage *msg)
{
    if (msg->isPacket())
    {
        auto packet = check_and_cast<inet::Packet *>(msg);
        auto serviceRequest = dynamic_cast<const ProxyServiceRequest *>(packet->peekData().get());

        // Registration or unregistration of a service
        if (serviceRequest)
            handleServiceRequest(packet, serviceRequest);
        else
            handleAppPacket(packet);
    }
    else
        handleAppCommand(check_and_cast<inet::Message *>(msg));
}

void AppProxy::handleServiceRequest(inet::Packet *packet, const ProxyServiceRequest *serviceRequest)
{
    if (serviceRequest->getOperation() == ProxyOperation::REGISTER)
    {
        serviceTable->registerService(serviceRequest->getService(), serviceRequest->getIp(), serviceRequest->getPort());
        
        // Update the info
        Connection &conn = socketToConnnectionMap.at(serviceRequest->getSocketId());
        conn.ip = serviceRequest->getIp();
        conn.port = serviceRequest->getPort();
    }
    else
    {
        serviceTable->unregisterService(serviceRequest->getService(), serviceRequest->getIp(), serviceRequest->getPort());
    }

    delete packet;
}

void AppProxy::handleAppPacket(inet::Packet *packet)
{
    send(packet, transportGateOut);
}

void AppProxy::handleAppCommand(inet::Message *message)
{
    cGate *inGate = message->getArrivalGate();
    const auto &socketReq = message->getTag<SocketReq>();
    TcpCommand *tcpCommand = dynamic_cast<TcpCommand *>(message->getControlInfo());

    if (tcpCommand == nullptr)
        error("Proxy module doesn't support non TCP commands");

    int socketReqId = socketReq->getSocketId();

    // Opening socket
    if (message->getKind() == TCP_C_OPEN_ACTIVE || message->getKind() == TCP_C_OPEN_PASSIVE)
    {
        auto it = socketToConnnectionMap.find(socketReqId);
        if (it != socketToConnnectionMap.end())
            throw cRuntimeError("Socket is already registered: socketId = %d, current gateIndex = %d, new gateIndex = %d, pathStartGate = %s, pathEndGate = %s",
                                socketReqId, it->second.gateIndex, inGate->getIndex(), inGate->getPathStartGate()->getFullPath().c_str(), inGate->getPathEndGate()->getFullPath().c_str());

        // Init the connection
        Connection connection;
        connection.socketId = socketReqId;
        connection.gateIndex = message->getArrivalGate()->getIndex();
        connection.type = message->getKind() == TCP_C_OPEN_PASSIVE ? ConnectionType::EDGE : ConnectionType::PASSTHROUGH;
        socketToConnnectionMap[socketReqId] = connection;

        EV_DEBUG << "New connection: " << connection << "\n";

        // On client/ephemeral conns then send to TCP layer
        if (message->getKind() == TCP_C_OPEN_ACTIVE)
            send(message, transportGateOut);
        else
            delete message;
    }
    // Connection has been accepted -> Belongs to a session!
    else if (message->getKind() == TCP_C_ACCEPT)
    {
        auto it = socketToSessionMap.find(socketReqId);
        if (it == socketToSessionMap.end())
            error("Connection not found: socketId = %d", socketReqId);

        cQueue *queue = it->second.setSessionEstablished();
        if (queue != nullptr)
        {
            while (!queue->isEmpty())
                send(check_and_cast<cMessage *>(queue->pop()), outGateBase + it->second.getGateIndex());
            delete queue;
        }
        delete message;
    }
    // Closing socket -- Could be associated to a connection or session!
    else if (message->getKind() == TCP_C_CLOSE)
    {
        auto it = socketToConnnectionMap.find(socketReqId);
        if (it != socketToConnnectionMap.end())
        {
            if (it->second.type == ConnectionType::EDGE)
                send(message, transportGateOut);
            else
                delete message;

            socketToConnnectionMap.erase(it);
        }
        else
        {
            auto it = socketToSessionMap.find(socketReqId);
            if (it != socketToSessionMap.end())
            {
                send(message, transportGateOut);
                socketToSessionMap.erase(it);
            }
            else
                error("Connection not found on CLOSE: socketId = %d", socketReqId);
        }
    }
}

void AppProxy::handleLowerMessage(cMessage *msg)
{
    if (msg->isPacket())
        handleTransportPacket(check_and_cast<inet::Packet *>(msg));
    else
        handleTransportIndication(check_and_cast<inet::Message *>(msg));
}

void AppProxy::handleTransportPacket(inet::Packet *packet)
{
    int socketId = packet->getTag<SocketInd>()->getSocketId();

    // Check if the session is established, buffer if not
    auto session = socketToSessionMap.find(socketId);

    if (session == socketToSessionMap.end() || session->second.getState() == SessionState::ESTABLISHED)
    {
        send(packet, outGateBase + findSocketGateIndex(socketId));
    }
    else if (session->second.getState() == SessionState::INIT)
    {
        // Case for when a session hasn't been bound yet to an endpoint
        Session &session = socketToSessionMap.at(socketId);
        std::string serviceName = getServiceName(packet);
        auto iter = serviceTable->findService(serviceName.c_str());

        if (iter == serviceTable->endOfServiceMap())
            sendServiceUnavailable(session.getSocketId(), packet);
        else
            handleSessionInitialize(session, iter->second, packet);
    }
    else
    {
        // Connection pending, buffer the packet
        session->second.pushToPendingQueue(packet);
    }
}

void AppProxy::handleSessionInitialize(Session &session, std::vector<ServiceEntry> &entries, inet::Packet *packet)
{
    // Select the endpoint ip + get the corresponding gate index
    int index = selectIp(entries);
    int ip = entries[index].getIp();
    int port = entries[index].getPort();

    Connection *connection = findConnection(ip, port);
    if (!connection)
        error("Connection not found: ip = %d, port = %d", ip, port);
    
    // Update the session entry
    inet::Message *message = session.setSessionPending(index);
    message->getTagForUpdate<SocketInd>()->setSocketId(connection->socketId);
    // Send a request to the endpoint to open a socket (we're using the same socketId)
    send(message, outGateBase + connection->gateIndex);
    // Keep the request in the pending queue
    session.pushToPendingQueue(packet);
}

void AppProxy::sendServiceUnavailable(int socketId, inet::Packet *packet)
{
    auto httpResponse = makeShared<RestfulResponse>();
    auto httpRequest = packet->peekData<RestfulRequest>();
    httpResponse->setHost(httpRequest->getHost());
    httpResponse->setPath(httpRequest->getPath());
    httpResponse->setResponseCode(ResponseCode::INTERNAL_ERROR);

    auto response = new inet::Packet("Service Unavailable", inet::TCP_C_SEND);
    response->insertData(httpResponse);
    response->addTag<SocketReq>()->setSocketId(socketId);
    response->addTag<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    send(response, transportGateOut);
    delete packet;
}

void AppProxy::handleTransportIndication(inet::Message *message)
{
    int socketId = message->getTag<SocketInd>()->getSocketId();

    // Main serving socket
    if (serverSocket.belongsToSocket(message))
    {
        if (message->getKind() != TCP_I_AVAILABLE)
            error("TCP_I_AVAILABLE expected on server socket :%d", serverSocket.getLocalPort());

        auto availableInfo = check_and_cast<TcpAvailableInfo *>(message->getControlInfo());

        int newSocket = availableInfo->getNewSocketId();
        serverSocket.accept(newSocket);

        // Take ownership
        socketToSessionMap.emplace(std::piecewise_construct, std::forward_as_tuple(newSocket), std::forward_as_tuple(newSocket, message));
        return;
    }

    // If it's for a connection then forward
    auto it = socketToConnnectionMap.find(socketId);
    if (it != socketToConnnectionMap.end())
    {
        send(message, outGateBase + it->second.gateIndex);
        return;
    }

    // If it's for a session then we need to look at it carefully
    auto it2 = socketToSessionMap.find(socketId);
    if (it2 == socketToSessionMap.end())
        error("Unkwon socketId: %d", socketId);

    // Already established forward the indication
    if (it2->second.getState() == SessionState::ESTABLISHED)
    {
        send(message, outGateBase + it2->second.getGateIndex());
        return;
    }

    // Session pending
    if (message->getKind() != TCP_I_ESTABLISHED)
        error("TBD handle TCP_CLOSED");

    delete message;
}

int AppProxy::findSocketGateIndex(int socketId)
{
    auto it = socketToConnnectionMap.find(socketId);
    if (it != socketToConnnectionMap.end())
        return it->second.gateIndex;

    auto it2 = socketToSessionMap.find(socketId);
    if (it2 != socketToSessionMap.end())
        return it2->second.getGateIndex();
    else
        error("No connection for socket %d", socketId);

    return -1;
}

int AppProxy::selectIp(std::vector<ServiceEntry> &entries)
{
    int index = intuniform(0, entries.size() - 1);
    return index;
}

std::string AppProxy::getServiceName(inet::Packet *packet)
{
    ChunkQueue data("Data processed", packet->peekData());

    // Look inside for the first REST like chunk in the data
    if (data.has<RestfulRequest>())
    {
        auto serviceRequest = data.peek<RestfulRequest>();
        std::string service = opp_removeend(serviceRequest->getHost(), serviceTable->getDomain());

        if (service.back() == '.')
            service.pop_back();

        return service;
    }
    else
        return "";
}

Connection *AppProxy::findConnection(int ip, int port)
{
    for (auto it = socketToConnnectionMap.begin(); it != socketToConnnectionMap.end(); it++)
    {
        if (it->second.ip == ip && it->second.port == port)
            return &(it->second);
    }
    return nullptr;
}
