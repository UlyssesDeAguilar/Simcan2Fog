#include "DnsRegistryService.h"
#include "DnsRegistrationRequest_m.h"

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

using namespace dns;

Define_Module(DnsRegistryService);

void DnsRegistryService::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {
        cache = check_and_cast<DnsCache*>(getModuleByPath("^.cache"));

        // statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.setOutputGate(gate("socketOut"));
        socket.bind(DNS_PORT);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");

        if ((int) par("mode") == NET_SCAN)
            scanNetwork();
    }
}

void DnsRegistryService::scanNetwork()
{
    cTopology topo;
    ResourceRecord r;
    r.type = ResourceRecord::RR_Type::A;

    topo.extractByProperty("servicenode");

    if (topo.getNumNodes() == 0)
        error("Couldn't find the nodes in the topology");

    for (int i = 0; i < topo.getNumNodes(); i++)
    {
        cTopology::Node *node = topo.getNode(i);
        cModule *module = node->getModule();
        L3Address address = L3AddressResolver().addressOf(module->getSubmodule("stack"));
        const char *domain = module->par("serviceDeployed");

        r.ip = address;
        r.domain = domain;        
        cache->insertData(r);
        EV << "Service: \"" << domain << "\" located on: " << module->getName() << " with ip: " << address << "\n";
    }
}

void DnsRegistryService::sendBack(cMessage *msg)
{
    Packet *packet = dynamic_cast<Packet *>(msg);

    if (packet)
    {
        msgsSent++;
        bytesSent += packet->getByteLength();
        emit(packetSentSignal, packet);

        EV_INFO << "sending \"" << packet->getName() << "\" to TCP, " << packet->getByteLength() << " bytes\n";
    }
    else
    {
        EV_INFO << "sending \"" << msg->getName() << "\" to TCP\n";
    }

    packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    send(msg, "socketOut");
}

void DnsRegistryService::handleMessage(cMessage *msg)
{
    if (msg->getKind() == TCP_I_PEER_CLOSED)
    {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        sendBack(request);
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA)
    {
        ChunkQueue queue;
        Packet *packet = check_and_cast<Packet *>(msg);

        int connId = packet->getTag<SocketInd>()->getSocketId();
        emit(packetReceivedSignal, packet);
        auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
        queue.push(chunk);

        // Process the requests which arrived
        while (queue.has<DnsRegistrationRequest>(b(-1)))
        {
            const auto &request = queue.pop<DnsRegistrationRequest>(b(-1));
            msgsRcvd++;
            bytesRcvd += B(request->getChunkLength()).get();

            // Process each binding action
            for (int i = 0; i < request->getBindingArraySize(); i++)
            {
                const DomainBinding &binding = request->getBinding(i);
                if (!binding.unregisterIfPresent)
                    cache->insertData(binding.record);
                else
                    ; // TODO: Implement an erasing method onto the dns cache
            }
        }
        delete msg;
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
    {
        socket.processMessage(msg);
    }
    else
    {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void DnsRegistryService::refreshDisplay() const
{
    char buf[64];
    sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void DnsRegistryService::finish()
{
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}
