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

#include "DnsQuerierService.h"
using namespace dns;

Define_Module(DnsQuerierService);

void DnsQuerierService::initialize(int stage)
{
    // This layer could be initialized earlier, maybe in INITSTAGE_TRANSPORT_LAYER
    if (stage == INITSTAGE_LOCAL)
    {
        serverGateId = gate("querierIn")->getId();
        cache = check_and_cast<DnsCache *>(getModuleByPath("^.cache"));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
    }
    ApplicationBase::initialize(stage);
}

uint16_t DnsQuerierService::getNewQueryId()
{
    if (queryId == UINT16_MAX)
        queryId = 0;
    return queryId++;
}

void DnsQuerierService::finish()
{
    // Clear cache for the next run
    serverSocket.close();
    queryId = 0;
}

void DnsQuerierService::handleStartOperation(LifecycleOperation *operation)
{
    // Prepare the socket for incoming data
    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(0);
}

void DnsQuerierService::handleMessageWhenUp(cMessage *msg)
{
    if (msg->getArrivalGate()->getId() == serverGateId)
        handleNewQuestion(msg);
    else
        serverSocket.processMessage(msg);
}

void DnsQuerierService::handleNewQuestion(cMessage *msg)
{
    const char *question = msg->getName();

    if (questions.find(question) != questions.end())
    {
        cStringTokenizer tokenizer(question, ".");
        std::ostringstream stream;

        auto vec = tokenizer.asVector();
        stream << vec[vec.size() - 1] << "." << vec.back();

        ParsedQuery *query = new ParsedQuery();
        query->domain = question;
        query->tld = vec.back();
        query->ns = stream.str().c_str();

        processQuestion(query);

        questions.insert(question);
    }

    delete msg;
}

void DnsQuerierService::processQuestion(ParsedQuery *query)
{
    // If we have the name server just query it
    if (cache->hasNsDomain(query->ns.c_str()))
    {
        auto request = sendRequest(query->domain.c_str(), cache->getNsCachedIp(query->ns.c_str()));
        delete query;
        return;
    }
    else if (cache->hasTldDomain(query->tld.c_str()))
        resolveNextStage(query->ns.c_str(), cache->getTldCachedIp(query->tld.c_str()), query);
    else
        resolveNextStage(query->tld.c_str(), ROOT_DNS_IP, query);
}

void DnsQuerierService::processNextStage(ParsedQuery *query, DependenciesList list)
{
    // If we have the name server just query it
    if (cache->hasNsDomain(query->ns.c_str()))
    {
        const inet::L3Address &ns = cache->getNsCachedIp(query->ns.c_str());
        sendRequest(std::move(list), ns);
    }
    else if (cache->hasTldDomain(query->tld.c_str()))
        resolveNextStage(query->ns.c_str(), cache->getTldCachedIp(query->tld.c_str()), std::move(list));
    else
        error("Moving from the highest level (querying ROOT) moves again to ROOT, something is clearly wrong");
}

void DnsQuerierService::resolveNextStage(const char *domain, const L3Address &targetIp, ParsedQuery *query)
{
    auto iter = pendingRequests.find(domain);
    if (iter == pendingRequests.end())
    {
        auto request = sendRequest(domain, targetIp);

        auto &entry = pendingRequests[domain];
        entry.id = request->getRequestId();
        entry.dependencies = std::unique_ptr<std::list<std::unique_ptr<ParsedQuery>>>(new std::list<std::unique_ptr<ParsedQuery>>());
        entry.dependencies->push_back(std::unique_ptr<ParsedQuery>(query));
    }
    else
    {
        iter->second.dependencies->push_back(std::unique_ptr<ParsedQuery>(query));
    }
}

void DnsQuerierService::resolveNextStage(const char *domain, const L3Address &targetIp, DependenciesList list)
{
    auto iter = pendingRequests.find(domain);
    if (iter == pendingRequests.end())
    {
        auto request = sendRequest(domain, targetIp);

        auto &entry = pendingRequests[domain];
        entry.id = request->getRequestId();
        entry.dependencies = std::move(list);
    }
    else
    {
        error("While moving dependencies queue, there was another queue already");
    }
}

DnsRequest *DnsQuerierService::sendRequest(const char *domain, const L3Address &address)
{
    auto request = new DnsRequest();
    request->setRequestId(getNewQueryId());
    request->setOperationCode(OP_Code::QUERY);
    request->appendQuestion(domain);

    auto packet = new Packet("DnsQuery", Ptr<DnsRequest>(request));
    serverSocket.sendTo(packet, address, DNS_PORT);
    return request;
}

DnsRequest *DnsQuerierService::sendRequest(DependenciesList list, const L3Address &address)
{
    auto request = new DnsRequest();
    request->setRequestId(getNewQueryId());
    request->setOperationCode(OP_Code::QUERY);

    for (const auto &question : *list)
        request->appendQuestion(question->domain.c_str());

    auto packet = new Packet("DnsQuery", Ptr<DnsRequest>(request));
    serverSocket.sendTo(packet, address, DNS_PORT);
    return request;
}

void DnsQuerierService::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // See if it is a DNS request
    auto request = dynamic_pointer_cast<const DnsRequest>(packet->peekData());

    if (!request)
        error("Recieved wrong package or type of chunk");

    if (request->isAuthoritative())
    {
        if (request->getReturnCode() == NOERROR)
        {
            for (int i = 0; i < request->getAuthoritativeAnswersArraySize(); i++)
            {
                const ResourceRecord &record = request->getAuthoritativeAnswers(i);
                cache->insertData(record);
                auto iter = pendingRequests.find(record.domain);
                pendingRequests.erase(iter);
                questions.erase(record.domain.c_str());

                // Ping the Querier Service as we've just got an authoritative answer
                send(new cMessage(record.domain.c_str(), NOERROR), gate("querierOut"));
            }
        }
        else
        {
            for (int i = 0; i < request->getQuestionArraySize(); i++)
            {
                const char *domain = request->getQuestion(i);
                auto iter = pendingRequests.find(domain);
                pendingRequests.erase(iter);
                questions.erase(domain);

                // Ping the Querier Service as we've just got an authoritative answer
                send(new cMessage(domain, NXDOMAIN), gate("querierOut"));
            }
        }
    }
    else
    {
        if (request->getNonAuthoritativeAnswersArraySize() == 0 || request->getReturnCode() == NXDOMAIN)
            error("No response for ns or tld");

        const ResourceRecord &record = request->getNonAuthoritativeAnswers(0);
        cache->insertData(record);
        auto iter = pendingRequests.find(record.domain);
        processNextStage(iter->second.dependencies->front().get(), std::move(iter->second.dependencies));
        pendingRequests.erase(iter);
    }

    delete packet;
}

void DnsQuerierService::socketErrorArrived(UdpSocket *socket, Indication *indication) {}
