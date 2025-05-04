//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "MessageQueueIn.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"

Define_Module(MessageQueueIn);

void MessageQueueIn::initialize(int stage) {
    if (stage == INITSTAGE_LOCAL){
        queueOut = check_and_cast<MessageQueueOut*>(getModuleByPath(par("messageQueueOutPath")));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        socket.setOutputGate(gate("socketOut"));
        socket.bind(
                localAddress[0] ?
                        L3AddressResolver().resolve(localAddress) : L3Address(),
                localPort);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus =
                node ? check_and_cast_nullable<NodeStatus*>(
                               node->getSubmodule("status")) :
                       nullptr;
        bool isOperational = (!nodeStatus)
                || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError(
                    "This module doesn't support starting in node DOWN state");
    }
}

void MessageQueueIn::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        EV_WARN << "Self message not used actually\n";
        delete msg;
    } else if (msg->getKind() == TCP_I_PEER_CLOSED) {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId =
                check_and_cast<Indication*>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        auto &tags = request->getTags();
        tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
        sendBack(request, connId);
    } else if (msg->getKind() == TCP_I_DATA
            || msg->getKind() == TCP_I_URGENT_DATA) {
        Packet *packet = check_and_cast<Packet*>(msg);
        int connId = packet->getTag<SocketInd>()->getSocketId();
        auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
        processingQueue.push(chunk);

        while (processingQueue.has<INET_AppMessage>(b(-1))) {
            const auto &appmsg = processingQueue.pop<INET_AppMessage>(b(-1));
            handleIncomingMessage(appmsg);
        }
        processingQueue.clear();
        delete msg;

    } else if (msg->getKind() == TCP_I_AVAILABLE) {
        socket.processMessage(msg);
    } else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind()
                       << "("
                       << cEnum::get("inet::TcpStatusInd")->getStringFor(
                               msg->getKind()) << ")\n";
        delete msg;
    }
}

void MessageQueueIn::handleIncomingMessage(const Ptr<const INET_AppMessage> &msg)
{
    auto sm = check_and_cast<const SIMCAN_Message*>(msg->getAppMessage());
    const char *destinationTopic = sm->getDestinationTopic();
    queueOut->pushIncomingMessage(destinationTopic, msg);
}

void MessageQueueIn::sendBack(cMessage *msg, int socketId) {
    auto packet = dynamic_cast<Packet*>(msg);
    if (packet) {
        packet->addTag<SocketReq>()->setSocketId(socketId);
        auto &tags = packet->getTags();
        tags.addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    }
    send(packet, "socketOut");
}
