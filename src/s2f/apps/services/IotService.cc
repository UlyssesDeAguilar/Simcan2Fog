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

#include "IotService.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/socket/SocketTag_m.h"

using namespace inet;
Define_Module(IotService);

void IotService::initialize() {
    AppBase::initialize();
    setReturnCallback(this);
    connectionQueue.clear();
    serviceName = this->appInstance;

    // Open the socket and listen for incoming comms
    sockFd = open(443, SOCK_STREAM);
    listen(sockFd);
}

void IotService::processSelfMessage(cMessage *msg) {
    if (msg->getKind() == EXEC_START) {
        registerService(serviceName, sockFd);
    } else if (msg->getKind() == SEND_DELAYED) {
        auto packet = check_and_cast<Packet*>(msg);
        EV << "Sending response to client" << "\n";
        _send(packet->getTag<SocketReq>()->getSocketId(), packet);
    }
    else {
        EV_WARN << "Unrecognized message of type " << msg->getClassName()
                       << "\n";
    }
}

void IotService::handleDataArrived(int sockFd, Packet *p) {
    EV << "Recieved packet: " << p->getName() << "\n";

    ChunkQueue &queue = connectionQueue.at(sockFd);
    queue.push(p->peekData());

    while (queue.has<RestfulRequest>(b(-1)))
    {
      EV_INFO << "Reassembled client request\n";
      auto request = queue.pop<RestfulRequest>(b(-1));
      auto packet = new Packet("Response");
      auto response = makeShared<RestfulResponse>();
      response->setResponseCode(ResponseCode::OK);
      response->setChunkLength(B(par("replySize")));
      response->addTag<CreationTimeTag>()->setCreationTime(simTime());
      response->setHost(serviceName);
      response->setPath(request->getPath());

      packet->insertData(response);
      packet->addTag<SocketReq>()->setSocketId(sockFd);
      packet->setKind(SEND_DELAYED);
      scheduleAt(simTime() + par("responseDelay"), packet);
    }

    delete p;
}

bool IotService::handleClientConnection(int sockFd, const L3Address &remoteIp,
        const uint16_t &remotePort) {
    EV << "Client connected: " << remoteIp << ":" << remotePort << "\n";
    EV << "Socket fd: " << sockFd << "\n";

    // Create the chunk queue
    connectionQueue[sockFd];
    return true;
}

bool IotService::handlePeerClosed(int sockFd) {
    EV << "Peer closed, session ended\n";
    connectionQueue.erase(sockFd);
    return true;
}
