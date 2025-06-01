#include "s2f/architecture/dns/registry/DnsRegistryService.h"

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
#include "s2f/architecture/net/protocol/RestfulResponse_m.h"

using namespace s2f::dns;
using namespace omnetpp;
using namespace inet;

Define_Module(DnsRegistryService);

void DnsRegistryService::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {
        dnsDatabase = check_and_cast<DnsDb *>(findModuleByPath(par("dbPath")));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(443);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");
    }
}

void DnsRegistryService::sendBack(cMessage *msg)
{
    Packet *packet = dynamic_cast<Packet *>(msg);

    if (packet)
    {
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
            auto newPacket = new Packet("DnsRegistry Response");

            bool ok = processRequest(request.get());
            auto response = makeShared<RestfulResponse>();

            response->setResponseCode(ok ? ResponseCode::OK : ResponseCode::INTERNAL_ERROR);
            response->setHost(request->getHost());
            response->setPath(request->getPath());

            newPacket->addTag<SocketReq>()->setSocketId(connId);
            newPacket->insertData(response);
            newPacket->setKind(TCP_C_SEND);

            sendBack(newPacket);
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

bool DnsRegistryService::processRequest(const DnsRegistrationRequest *request)
{
    if (request->getVerb() == Verb::PUT)
    {
        EV_INFO << "Adding " << request->getRecordsArraySize() << " records\n";
        for (int i = 0; i < request->getRecordsArraySize(); i++)
            dnsDatabase->insertRecord(request->getZone(), request->getRecords(i));

        return true;
    }
    else if (request->getVerb() == Verb::DELETE)
    {
        EV_INFO << "Removing " << request->getRecordsArraySize() << " records\n";
        for (int i = 0; i < request->getRecordsArraySize(); i++)
            dnsDatabase->removeRecord(request->getZone(),request->getRecords(i));

        return true;
    }
    else
    {
        EV_WARN << "Unable to process request with verb: " << request->getVerb() << "\n";
        return false;
    }
}
