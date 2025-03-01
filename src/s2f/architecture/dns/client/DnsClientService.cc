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

#include "s2f/architecture/dns/client/DnsClientService.h"
#include "inet/common/packet/ChunkQueue.h"

using namespace dns;
using namespace omnetpp;
using namespace inet;

Define_Module(DnsClientService);

DnsClientService::DnsClientService() { tickTimer = new cMessage("Tick timer"); }
DnsClientService::~DnsClientService() { delete tickTimer; }

void DnsClientService::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        ticksForTimeOut = par("ticksForTimeOut");
        timePerTick = par("timePerTick");
        maxAttempts = par("maxAttempts");
    }

    ApplicationBase::initialize(stage);
}

void DnsClientService::finish()
{
    // Clear cache for the next run
    socket.close();
    cancelEvent(tickTimer);
    requestMap.clear();
    ApplicationBase::finish();
}

void DnsClientService::handleStartOperation(LifecycleOperation *operation)
{
    // Prepare the socket for incoming data
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
    socket.bind(-1);
}

void DnsClientService::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        rescheduleAt(simTime() + simtime_t(timePerTick, SIMTIME_MS), tickTimer);
        processRequests();
        return;
    }

    // Check if it's a new request or a socket communication
    cGate *incomingGate = msg->getArrivalGate();
    if (incomingGate == gate("clientIn"))
    {
        auto command = check_and_cast<DnsClientCommand *>(msg);
        handleRequest(command);
    }
    else
        socket.processMessage(msg);
}

void DnsClientService::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    ChunkQueue data("Incoming data", packet->peekData());

    if (data.has<DnsRequest>())
    {
        auto dnsPacket = data.pop<DnsRequest>();
        auto iter = requestMap.find(dnsPacket->getRequestId());

        // In this case we got a response! Done with the request
        if (iter != requestMap.end())
        {
            sendResponse(dnsPacket->getRequestId(), OK, packet);
            requestMap.erase(iter);
            return;
        }
        else
            EV_WARN << "Ignoring response with id: " << dnsPacket->getRequestId() << "\n";
    }

    delete packet;
}

void DnsClientService::handleRequest(DnsClientCommand *request)
{
    // Push onto the map
    auto iter = requestMap.emplace(request->getRequestId(), request);
    if (iter.second)
    {
        // Send the request
        DnsClientService::RequestContext &ctx = iter.first->second;
        sendRequest(ctx);
    }
    else
        EV_INFO << "DnsClientService: request with id: " << request->getRequestId() << " already exists\n";
    
    delete request;
}

bool DnsClientService::sendRequest(RequestContext &ctx)
{
    bool shouldContinue = true;

    // Avoid servers that have failed
    while (failedServers.find(ctx.servers[ctx.selectedAddress]) != failedServers.end() && shouldContinue)
        shouldContinue = ctx.updateWithFailedAttempt(0);

    if (shouldContinue)
    {
        socket.sendTo(ctx.packet->dup(), ctx.servers[ctx.selectedAddress], DNS_PORT);
        ctx.tries++;
    }

    return shouldContinue;
}

void DnsClientService::sendResponse(uint16_t id, DnsClientResult status, inet::Packet *packet)
{
    ASSERT2(status == OK && packet != nullptr, "Error, cannot send OK with empty payload!");
    auto response = new DnsClientIndication();
    response->setRequestId(id);
    response->setResult(status);
    response->setPayload(packet);
    send(response, "clientOut");
}

void DnsClientService::processRequests()
{
    // Update on the requests
    for (auto iter = requestMap.begin(); iter != requestMap.end();)
    {
        iter->second.tick++;

        // In case we timed out
        if (iter->second.tick == ticksForTimeOut)
        {
            // See if we can continue
            iter->second.tick = 0;
            if (iter->second.updateWithFailedAttempt(maxAttempts) && sendRequest(iter->second))
                ++iter;
            else
            {
                sendResponse(iter->second.id, ERROR);
                iter = requestMap.erase(iter);
            }
        }
        else
            // Otherwise, just move on while we wait
            ++iter;
    }

    // Clear failed server list
    failedServers.clear();
}

void DnsClientService::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    auto &remoteAddress = indication->getTag<L3AddressInd>()->getDestAddress();
    auto remotePort = indication->getTag<L4PortInd>()->getDestPort();

    EV_INFO << "Failed reaching server: " << remoteAddress << ":" << remotePort << "\n";
    failedServers.insert(remoteAddress);
    delete indication;
}
