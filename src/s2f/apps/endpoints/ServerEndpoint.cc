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

#include "ServerEndpoint.h"

using namespace inet;

Define_Module(ServerEndpoint);

void ServerEndpoint::initialize()
{
    AppBase::initialize();
    setReturnCallback(this);
    
    if (par("useAppIdAsServiceName"))
        serviceName = this->appInstance;
    else
        serviceName = par("serviceName");
    
    // Open the socket and listen for incoming comms
    sockFd = open(443, SOCK_STREAM);
    listen(sockFd);
}

void ServerEndpoint::processSelfMessage(cMessage *msg) 
{
    if (event == msg)
    {
        // Register the service
        registerService(serviceName, sockFd);
    }
    else
        delete msg;
}

void ServerEndpoint::handleDataArrived(int sockFd, Packet *p) 
{
    EV << "Recieved packet: " << p->getName() << "\n";
    EV << "Sending response to client" << "\n";

    auto packet = new Packet("Request");
    
    auto request = makeShared<RestfulResponse>();
    request->setResponseCode(ResponseCode::OK);
    request->setHost(serviceName);
    request->setPath(dynamic_pointer_cast<const RestfulRequest>(p->peekData())->getPath());
    packet->insertData(request);
    _send(sockFd, packet);
    
    delete p;
}

bool ServerEndpoint::handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort)
{
    EV << "Client connected: " << remoteIp << ":" << remotePort << "\n";
    EV << "Socket fd: " << sockFd << "\n";
    return true;
}

bool ServerEndpoint::handlePeerClosed(int sockFd)
{
    EV << "Peer closed, session ended\n";
    return true;
}

