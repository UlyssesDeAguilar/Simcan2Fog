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

#include "ProbingClient.h"

using namespace inet;

Define_Module(ProbingClient);

#define SEND_PROBE 100

void ProbingClient::initialize()
{
    AppBase::initialize();
    setReturnCallback(this);

    // Open the ephemeral socket
    sockFd = open(-1, SOCK_STREAM);
    started = false;
}

void ProbingClient::processSelfMessage(cMessage *msg)
{
    if (event == msg)
    {
        attemptStart();
    }
    else if (msg->getKind() == SEND_PROBE)
    {
        sendProbe();
        delete msg;
    }
    else
        delete msg;
}

void ProbingClient::attemptStart()
{
    if (!started)
    {
        if (opp_isempty(par("serviceName")))
            return;

        if (resolverGate != nullptr)
        {
            resolve(par("serviceName"));
        }
        else
            error("No resolver available and not able to get an Ip address for the service host param ... giving up!");
        
        started = true;
    }
}

void ProbingClient::handleResolutionFinished(const L3Address ip, bool resolved)
{
    if (!resolved)
        error("Unexpected unavailability of service: %s", (const char *)par("serviceName"));

    EV << "Resolving service: " << par("serviceName") << " yielded: " << ip << "\n";
    EV << "Starting connection with service: ..." << "\n";
    connect(sockFd, ip, 443);
}

void ProbingClient::handleConnectReturn(int sockFd, bool connected)
{
    if (!connected)
        error("Unable to connect to service");

    EV << "Connected to service: " << par("serviceName") << "\n";
    EV << "Sending request to service" << "\n";
    sendProbe();
}

void ProbingClient::handleDataArrived(int sockFd, Packet *p)
{
    EV << "Recieved packet: " << p->getName() << "\n";
    auto httpResponse = p->peekData<RestfulResponse>();

    if (httpResponse->getResponseCode() == ResponseCode::OK)
    {
        EV << "Sending request to the service" << "\n";
        sendRequest();
    }
    else
    {
        EV << "Service not ready, scheduling probe\n";
        scheduleAt(simTime() + par("pollingInterval"), new cMessage("Probe timer", SEND_PROBE));
    }

    delete p;
}

void ProbingClient::sendRequest()
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

void ProbingClient::sendProbe()
{
    // Send a request to the service
    auto packet = new Packet("Request (Probe)");
    auto request = makeShared<RestfulRequest>();
    request->setVerb(Verb::HEAD);
    request->setHost(par("serviceName"));
    request->setPath("/");
    packet->insertData(request);

    _send(sockFd, packet);
}

bool ProbingClient::handlePeerClosed(int sockFd)
{
    EV << "Peer closed, session ended\n";
    return true;
}

void ProbingClient::handleParameterChange(const char *parameterName)
{
    if (strcmp(parameterName, "serviceName") == 0)
        attemptStart();
}