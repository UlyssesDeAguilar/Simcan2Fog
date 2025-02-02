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

#include "ClientEndpoint.h"

using namespace inet;

Define_Module(ClientEndpoint);

void ClientEndpoint::initialize()
{
    AppBase::initialize();
    setReturnCallback(this);

    // Open the ephemeral socket
    sockFd = open(-1, SOCK_STREAM);
}

void ClientEndpoint::processSelfMessage(cMessage *msg)
{
    if (event == msg)
    {
        // Process if we have an Ip address or FQDN
        L3Address ip;
        if (ip.tryParse(par("servingHost")))
        {
            connect(sockFd, ip, 443);
        }
        else if (resolver != nullptr)
        {
            resolve(par("servingHost"));
        }
        else
            error("No resolver available and not able to get an Ip address for the service host param ... giving up!");
    }
    else
        delete msg;
}

void ClientEndpoint::handleResolutionFinished(const L3Address ip, bool resolved)
{
    if (!resolved)
        error("Unexpected unavailability of service: %s", (const char*) par("servingHost"));

    EV << "Resolving service: " << par("servingHost") << " yielded: " << ip << "\n";
    EV << "Starting connection with service: ..." << "\n";
    connect(sockFd, ip, 443);
}

void ClientEndpoint::handleConnectReturn(int sockFd, bool connected) 
{
    if (!connected)
        error("Unable to connect to service");
    
    EV << "Connected to service: " << par("servingHost") << "\n";
    EV << "Sending request to service" << "\n";
    sendRequest();
}

void ClientEndpoint::handleDataArrived(int sockFd, Packet *p)
{
    EV << "Recieved packet: " << p->getName() << "\n";
    EV << "Sending another request to the service" << "\n";
    sendRequest();
    delete p;
}

void ClientEndpoint::sendRequest()
{
    // Send a request to the service
    auto packet = new Packet("Request");
    auto request = makeShared<RestfulRequest>();
    request->setVerb(Verb::GET);
    request->setHost(par("serviceName"));
    request->setPath("/");
    packet->insertData(request);
    
    _send(sockFd, packet);
}

bool ClientEndpoint::handlePeerClosed(int sockFd) 
{
    EV << "Peer closed, session ended\n";
    return true;
}
