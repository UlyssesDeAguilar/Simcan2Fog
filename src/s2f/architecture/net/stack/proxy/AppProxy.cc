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

#include "s2f/architecture/net/stack/dispatcher/ToggleReq_m.h"
#include "s2f/apps/AppBase.h"

Define_Module(AppProxy);

void AppProxy::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void AppProxy::finish()
{
    // Release memory from the pending socket structs
    for (const auto &iter : pendingComms.getMap())
        delete iter.second;
}

// Service pool related
void AppProxy::bindService(const char *serviceName, AppBase *app)
{
    ServiceEntry entry;
    entry.app = app;
    entry.availableConnections = 1;

    auto &pool = serviceMap[serviceName];
    pool.emplace_back(entry);
}

void AppProxy::unbindService(const char *serviceName, AppBase *app)
{
    auto &pool = serviceMap.at(serviceName);

    auto finder = [app](const ServiceEntry &e) -> bool
    { return e.app == app; };

    auto iter = std::find_if(pool.begin(), pool.end(), finder);

    if (iter == pool.end())
        return;

    // As we don't want order this is the way to delete the element in O(1)
    std::iter_swap(pool.end() - 1, iter);
    pool.pop_back();
}

AppBase *AppProxy::findFitForRequest(const inet::Ptr<const RestMsg> &request)
{
    auto iter = serviceMap.find(request->getHostname());

    if (iter != serviceMap.end())
        return nullptr;

    auto &pool = iter->second;

    auto finder = [](const ServiceEntry &e) -> bool
    { return e.availableConnections > 0; };

    auto pool_iter = std::find_if(pool.begin(), pool.end(), finder);

    if (pool_iter == pool.end())
        return nullptr;

    pool_iter->availableConnections--;
    return pool_iter->app;
}

void AppProxy::notifyConnRelease(const char *serviceName, AppBase *app)
{
    auto &pool = serviceMap.at(serviceName);

    auto finder = [app](const ServiceEntry &e) -> bool
    { return e.app == app; };

    auto iter = std::find_if(pool.begin(), pool.end(), finder);

    if (iter == pool.end())
        return;

    iter->availableConnections++;
}

// Network related

void AppProxy::handleMessage(cMessage *msg)
{
    // Main server socket
    if (socket.belongsToSocket(msg))
    {
        if (msg->getKind() == TCP_I_AVAILABLE)
        {
            // We have to accept the connection until we know which is the route to be reached
            auto availableInfo = check_and_cast<TcpAvailableInfo *>(msg->getControlInfo());
            TcpSocket *newSocket = new TcpSocket(availableInfo);
            newSocket->setOutputGate(gate("socketOut"));
            newSocket->setCallback(this);
            pendingComms.addSocket(newSocket);
            socket.accept(availableInfo->getNewSocketId());
        }
        else
        {
            // some indication -- ignore
            EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
            delete msg;
        }
    }
    else
    {
        // For pending comms to be binded
        auto socket = pendingComms.findSocketFor(msg);
        if (socket)
            socket->processMessage(msg);
        else
            error("Message arrived for an unregistered socket");
    }
}

void AppProxy::socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent)
{
    int connId = socket->getSocketId();

    ChunkQueue queue;
    auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
    queue.push(chunk);
    emit(packetReceivedSignal, packet);

    if (!queue.has<RestMsg>(b(-1)))
        return;

    const auto &request = queue.peek<RestMsg>(b(-1));

    AppBase *app = findFitForRequest(request);

    // Find which gate vector index has the app
    cGate *g = app->gate("socketOut")->getPathEndGate();

    // Redirect the SocketInd packets and messages from the transport layer
    auto toggleRequest = new ToggleReq("Toggle socket request");
    toggleRequest->setSocketId(connId);
    toggleRequest->setGateIndex(g->getIndex());

    send(toggleRequest, gate("socketOut"));

    // Rebind socket and remove from pending comms
    app->acceptSocket(socket, packet);
    pendingComms.removeSocket(socket);
}

void AppProxy::socketPeerClosed(TcpSocket *socket)
{
    // Sadly, connection dropped before being served
    socket->close();
    pendingComms.removeSocket(socket);
    delete socket;
}
