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

#include "s2f/architecture/dns/resolver/DnsResolverService.h"
#include "inet/common/packet/ChunkQueue.h"

using namespace s2f::dns;
using namespace omnetpp;
using namespace inet;

Define_Module(DnsResolverService);

void DnsResolverService::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        dnsDatabase = check_and_cast<DnsDb *>(findModuleByPath(par("dbPath")));
        lastId = 0;
        lastQuestionId = 0;

        WATCH_MAP(resolutions);
        WATCH_MAP(questionMap);
        //WATCH_MAP(requestMap);
        WATCH(lastId);
        WATCH(lastQuestionId);
    }
    ApplicationBase::initialize(stage);
}

void DnsResolverService::finish()
{
    // Clear cache for the next run
    serverSocket.close();
    questionMap.clear();
    requestMap.clear();
    resolutions.clear();
    ApplicationBase::finish();
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
    cGate *incomingGate = msg->getArrivalGate();

    if (incomingGate == gate("serverIn"))
        handleResponse(check_and_cast<DnsClientIndication *>(msg));
    else
        serverSocket.processMessage(msg);
}

void DnsResolverService::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    ChunkQueue data("Incoming data", packet->peekData());
    while (data.has<DnsRequest>())
    {
        auto dnsPacket = data.pop<DnsRequest>();
        handleRequest(packet, dnsPacket);
    }
    delete packet;
}

void DnsResolverService::handleRequest(Packet *packet, Ptr<const DnsRequest> request)
{
    // Retrieve the information of the sender
    L3Address remoteAddress = packet->getTag<L3AddressInd>()->getSrcAddress();
    int remotePort = packet->getTag<L4PortInd>()->getSrcPort();
    Resolution resolution(lastId++, remoteAddress, remotePort, request);
    int questionCount = request->getQuestionArraySize();

    // If there are no questions or more than the practical ones return an error
    // Meanwhile the RFC 1035 specifies more than one question, in the real world it is either ignored or treated as an error.
    if (questionCount == 0 || questionCount > 1)
    {
        EV_WARN << "Query with unsopported number of questions: FORMERR" << "\n";
        resolution.response->setReturnCode(ReturnCode::FORMERR);
        makeTransition(resolution, RESOLVED);
        sendResponse(resolution);
        return;
    }

    // Process the resolution
    processResolution(resolution);

    // In case the resolution is finished, don't add to the DB!
    if (!resolution.hasFinished())
        resolutions[resolution.id] = resolution;
}

void DnsResolverService::handleResponse(DnsClientIndication *msg)
{
    auto iter = requestMap.find(msg->getRequestId());
    if (iter == requestMap.end())
    {
        delete msg;
        error("Received a response for an unknown request");
    }

    Packet *packet = msg->getPayloadForUpdate();
    std::set<uint16_t> &resolutionIds = iter->second;

    if (msg->getResult() != OK)
    {
        markFailedResolutions(resolutionIds, ReturnCode::SERVFAIL);
        delete msg;
        return;
    }

    ASSERT2(packet != nullptr, "Error, receive send OK with empty payload!");
    auto response = packet->peekData<DnsRequest>();

    if (response->getReturnCode() == ReturnCode::NOERROR)
    {
        const char *zone = response->getQuestion(0).getDomain();

        for (int i = 0; i < response->getNonAuthoritativeAnswerArraySize(); i++)
            dnsDatabase->insertRecord(zone, response->getNonAuthoritativeAnswer(i));

        for (int i = 0; i < response->getAuthoritativeAnswerArraySize(); i++)
            dnsDatabase->insertRecord(zone, response->getAuthoritativeAnswer(i));

        // Reprocess the affected resolutions
        for (auto &resolutionId : resolutionIds)
            processResolution(resolutions[resolutionId]);
    }
    else
    {
        markFailedResolutions(resolutionIds, (ReturnCode)response->getReturnCode());
    }

    // Release the entry, message
    requestMap.erase(iter);
    questionMap.erase(response->getQuestion(0).getDomain());
    delete msg;
}

void DnsResolverService::processResolution(Resolution &resolution, bool recursion)
{
    const DnsTreeNode *node = dnsDatabase->searchRecords(resolution.current_question);

    // Records not found even in best effort mode!
    if (node == nullptr || node->getRecords() == nullptr)
    {
        makeTransition(resolution, NOT_FOUND);
        return;
    }

    // We have records from the authoritative server
    if (node->getDnsLevel() == DnsLevel::AUTHORITATIVE)
        processAuthResolution(resolution, node);
    else
    {
        makeTransition(resolution, node->getDnsLevel() == DnsLevel::ROOT ? TLD_RESOLUTION : NS_RESOLUTION);
        registerQuestion(resolution.current_question, node, resolution.id);
    }

    // Clear if we ended
    if (resolution.hasFinished() && !recursion)
        resolutions.erase(resolution.id);
}

void DnsResolverService::processAuthResolution(Resolution &resolution, const DnsTreeNode *node)
{
    auto records = node->getRecords();

    for (auto iter = records->begin(); iter != records->end(); ++iter)
    {
        const ResourceRecord &record = *iter;
        if (matchWithWildcard(record, resolution.getCurrentQuestion()))
        {
            if (record.type == CNAME)
            {
                // Change the question, resolve again!
                resolution.current_question.setDomain(record.contents.c_str());
                processResolution(resolution, true);
                return;
            }
            else
            {
                // In other cases, add the answer we got!
                resolution.response->appendNonAuthoritativeAnswer(record);
            }
        }
    }

    // If we have answers, we are done
    if (resolution.response->getNonAuthoritativeAnswerArraySize() > 0)
        makeTransition(resolution, RESOLVED);
    else if (resolution.state == INIT)
    {
        makeTransition(resolution, AUTH_RESOLUTION);
        registerQuestion(resolution.current_question, node, resolution.id);
    }
    else
        makeTransition(resolution, NOT_FOUND);
}

void DnsResolverService::registerQuestion(const DnsQuestion &question, const DnsTreeNode *node, uint16_t id)
{
    // Pose the question at the level we are right now
    opp_string q = getQuestionForLevel(question.getDomain(), node->getDnsLevel());

    // If already "asked", wait for resolution
    auto iter = questionMap.find(q);
    if (iter != questionMap.end())
    {
        requestMap[iter->second].insert(id);
        return;
    }

    // Otherwise, register the question
    questionMap[q] = id;
    requestMap[id].insert(id);

    // Create the query command
    auto command = new DnsClientCommand();
    command->setRequestId(lastQuestionId++);

    // Prepare the payload
    auto request = makeShared<DnsRequest>();
    request->setOperationCode(QUERY);
    request->setRequestId(command->getRequestId());
    request->appendQuestion(question);
    auto packet = new Packet("DNS Request");
    packet->insertData(request);

    // Add the payload
    command->setPayload(packet);

    // Obtain the DNS server ips to be queried
    if (node->getDnsLevel() == DnsLevel::AUTHORITATIVE)
    {
        auto tldNode = node->getParent();
        auto records = tldNode->getRecords();

        // Get the right authoritative NS!
        for (auto record : *records)
            if (record.type == NS && matchesAuthorityDomain(record, question.getDomain()))
                command->appendIpPool(record.ip);
    }
    else
    {
        auto records = node->getRecords();
        for (auto record : *records)
        {
            if (record.type == NS)
                command->appendIpPool(record.ip);
        }
    }

    ASSERT2(command->getIpPoolArraySize() > 0, "Error, no IPs to query");
    send(command, "serverOut");
}

void DnsResolverService::markFailedResolutions(const std::set<uint16_t> &failedResolutions, ReturnCode returnCode)
{
    for (auto &id : failedResolutions)
    {
        auto iter = resolutions.find(id);
        Resolution &resolution = iter->second;
        resolution.response->setReturnCode(returnCode);
        makeTransition(resolution, NOT_FOUND);
        resolutions.erase(iter);
    }
}

void DnsResolverService::sendResponse(Resolution &resolution)
{
    // Sanity check
    if (!resolution.hasFinished())
        error("Attempting to send response(s) for a resolution that is not finished");

    ReturnCode code;
    if (resolution.state == RESOLVED)
        code = ReturnCode::NOERROR;
    else if(resolution.state == NOT_FOUND)
        code = ReturnCode::NXDOMAIN;
    else
        code = ReturnCode::SERVFAIL;

    resolution.response->setReturnCode(code);
    resolution.response->setIsAuthoritative(false);
    auto packet = new Packet("DNS Response", resolution.response);
    serverSocket.sendTo(packet, resolution.remoteAddress, resolution.remotePort);
}

void DnsResolverService::makeTransition(Resolution &resolution, ResolutionState state)
{
    EV_DEBUG << "Processing resolution with id: " << resolution.id
             << " state: " << resolution.stateToString(resolution.state)
             << " -> " << resolution.stateToString(state) << "\n";

    resolution.state = state;

    if (resolution.hasFinished())
        sendResponse(resolution);
}

void DnsResolverService::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << "\n";
    delete indication;
}
