#include "ProcessingService.h"

#include "Core/DataManager/DataManager.h"
#include "Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"

#include "inet/applications/common/SocketTag_m.h"
#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

simsignal_t connectionsSignal = cComponent::registerSignal("connections");
simsignal_t capacitySignal = cComponent::registerSignal("capacity");

Define_Module(ProcessingService);

void ProcessingService::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {
        delay = par("replyDelay");
        maxMsgDelay = 0;

        // statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = concurrentConnections = 0;

        minimumVms = par("minimumVms");
        scalingThreshold = par("scalingThreshold");
        numVms = minimumVms;

        if (minimumVms < 1)
            error("The (initial) pool size cannot be less than 1!");

        // managers
        resourceManager = check_and_cast<DcResourceManager *>(getModuleByPath(par("resourceManagerPath")));
        auto dataManager = check_and_cast<DataManager *>(getModuleByPath("simData.manager"));

        vm = dataManager->searchVirtualMachine(par("vmType"));

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
        socket.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");

        // Allocate the minimum vms
    }
}

void ProcessingService::sendOrSchedule(cMessage *msg, simtime_t delay)
{
    if (delay == 0)
        sendBack(msg);
    else
        scheduleAt(simTime() + delay, msg);
}

void ProcessingService::sendBack(cMessage *msg)
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

    auto &tags = getTags(msg);
    tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    send(msg, "socketOut");
}

void ProcessingService::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        sendBack(msg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED)
    {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        updateConnections(-1);
        sendOrSchedule(request, delay + maxMsgDelay);
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA)
    {
        Packet *packet = check_and_cast<Packet *>(msg);
        int connId = packet->getTag<SocketInd>()->getSocketId();
        ChunkQueue &queue = socketQueue[connId];
        auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
        queue.push(chunk);
        emit(packetReceivedSignal, packet);

        bool doClose = false;
        while (const auto &appmsg = queue.pop<GenericAppMsg>(b(-1), Chunk::PF_ALLOW_NULLPTR))
        {
            msgsRcvd++;
            bytesRcvd += B(appmsg->getChunkLength()).get();
            B requestedBytes = appmsg->getExpectedReplyLength();
            simtime_t msgDelay = appmsg->getReplyDelay();
            if (msgDelay > maxMsgDelay)
                maxMsgDelay = msgDelay;

            if (requestedBytes > B(0))
            {
                Packet *outPacket = new Packet(msg->getName(), TCP_C_SEND);
                outPacket->addTag<SocketReq>()->setSocketId(connId);
                const auto &payload = makeShared<GenericAppMsg>();
                payload->setChunkLength(requestedBytes);
                payload->setExpectedReplyLength(B(0));
                payload->setReplyDelay(0);
                payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
                outPacket->insertAtBack(payload);
                sendOrSchedule(outPacket, delay + msgDelay);
            }
            if (appmsg->getServerClose())
            {
                doClose = true;
                break;
            }
        }
        delete msg;

        if (doClose)
        {
            auto request = new Request("close", TCP_C_CLOSE);
            TcpCommand *cmd = new TcpCommand();
            request->addTag<SocketReq>()->setSocketId(connId);
            request->setControlInfo(cmd);
            updateConnections(-1);
            sendOrSchedule(request, delay + maxMsgDelay);
        }
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
    {
        //auto availableInfo = check_and_cast<TcpAvailableInfo *>(msg->getControlInfo());
        //availableInfo->getNewSocketId();
        socket.processMessage(msg);
        updateConnections(1);
    }
    else
    {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void ProcessingService::refreshDisplay() const
{
    char buf[64];
    sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void ProcessingService::finish()
{
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}

std::tuple<long, long> ProcessingService::getCurrentRange()
{
    if (numVms == minimumVms)
        return std::make_tuple(0, minimumVms * scalingThreshold);

    long upper_range = numVms * scalingThreshold;
    long lower_range = (numVms - 1) * scalingThreshold;

    return std::make_tuple(lower_range, upper_range);
}

void ProcessingService::updateConnections(long amount)
{
    // Update and emit
    concurrentConnections += amount;
    emit(connectionsSignal, concurrentConnections);

    auto range = getCurrentRange();
    if (std::get<1>(range) < concurrentConnections)
    {
        // Scale up
        numVms++;
        resourceManager->emitSignals(vm, true);
    }
    else if (std::get<0>(range) > concurrentConnections)
    {
        // Scale down
        numVms--;
        resourceManager->emitSignals(vm, false);
    }

    emit(capacitySignal, (double)concurrentConnections / (numVms * scalingThreshold * 1.0));
}
