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

#include "DnsResolverService.h"
using namespace dns;

Define_Module(DnsResolverService);

void DnsResolverService::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        cache = check_and_cast<DnsCache *>(getModuleByPath("^.cache"));
        serverInId = gate("serverIn")->getId();
    }
    ApplicationBase::initialize(stage);
}

void DnsResolverService::finish()
{
    // Clear cache for the next run
    serverSocket.close();
    pendingRequests.clear();
}

void DnsResolverService::handleStartOperation(LifecycleOperation *operation)
{
    // Prepare the socket for incoming data
    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(DNS_PORT);
}

void DnsResolverService::handleMessageWhenUp(cMessage *msg)
{
    if (msg->getArrivalGate()->getId() == serverInId)
        handleUpdateFromQuerier(msg);
    else
        serverSocket.processMessage(msg);
}

void DnsResolverService::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // See if it is a DNS request
    auto request = dynamic_cast<const DnsRequest *>(packet->peekData().get());

    if (request == nullptr)
        error("Recieved unkown packet");

    if (request->getOperationCode() != QUERY || request->getQuestionArraySize() > 1)
        error("What");

    const char *domain = request->getQuestion(0);

    if (cache->hasDomainCached(domain))
    {
        sendResponse(domain, packet, NOERROR);
        delete packet;
    }
    else
    {
        pendingRequests[domain].insert(packet);
        send(new cMessage(domain), gate("serverOut"));
    }
}

void DnsResolverService::handleUpdateFromQuerier(cMessage *msg)
{
    auto iter = pendingRequests.find(msg->getName());

    while (iter->second.getLength() > 0)
    {
        auto packet = check_and_cast<Packet *>(iter->second.pop());
        sendResponse(msg->getName(), packet, (ReturnCode)(msg->getKind()));
        delete packet;
    }

    // Deleting an element in a map only invalidates the iterator of the element being deleted
    pendingRequests.erase(iter);
    delete msg;
}

void DnsResolverService::sendResponse(const char *domain, Packet *packet, ReturnCode code)
{
    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());

    // Retrieve IP:Port from sender
    L3Address remoteAddress = packet->getTag<L3AddressInd>()->getSrcAddress();
    int remotePort = packet->getTag<L4PortInd>()->getSrcPort();
    // Process and package response
    auto response = request->dup();
    response->setIsAuthoritative(false);
    response->setReturnCode(code);
    
    if (code == NOERROR)
        response->insertNonAuthoritativeAnswers(cache->getCachedDomain(domain));
    
    auto responsePacket = new Packet("DnsRequest", Ptr<DnsRequest>(response));

    // Send to the requester
    serverSocket.sendTo(responsePacket, remoteAddress, remotePort);
}

void DnsResolverService::socketErrorArrived(UdpSocket *socket, Indication *indication) {}