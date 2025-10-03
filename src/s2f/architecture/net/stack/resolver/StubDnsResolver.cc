#include "s2f/architecture/net/stack/resolver/StubDnsResolver.h"

using namespace omnetpp;
using namespace inet;
using namespace s2f::dns;

Define_Module(StubDnsResolver);

void StubDnsResolver::initialize()
{
    ispResolver = L3Address(par("ispResolver"));
    dnsDatabase = check_and_cast<DnsDb *>(findModuleByPath(par("dbPath")));
}

void StubDnsResolver::finish()
{
    pendingRequests.clear();
}

void StubDnsResolver::handleMessage(cMessage *msg)
{
    auto arrivalGate = msg->getArrivalGate();
    if (arrivalGate == gate("clientIn"))
        handleRequest(check_and_cast<StubDnsRequest *>(msg));
    else
        handleResponse(check_and_cast<DnsClientIndication *>(msg));
}

void StubDnsResolver::handleRequest(StubDnsRequest *request)
{
    // Store context
    RequestContext ctx;
    ctx.id = lastId++;
    ctx.request = request;

    // Search for the record
    DnsQuestion question;
    question.setDomain(request->getDomain());
    question.setQuestionType(RecordType::A);
    const DnsTreeNode *node = dnsDatabase->searchRecords(question);

    // We might have the record cached
    if (node && node->getDnsLevel() == AUTHORITATIVE && node->getChildCount() > 0)
    {
        std::set<ResourceRecord> cachedRecords;

        std::copy_if(node->getRecords()->begin(), node->getRecords()->end(),
                     std::inserter(cachedRecords, cachedRecords.end()),
                     [question](const ResourceRecord &rr)
                     {
                         return matchWithWildcard(rr, question.getDomain());
                     });

        if (!cachedRecords.empty())
        {
            sendResponse(ctx, cachedRecords);
            return;
        }
    }

    // Generate request
    const auto &dnsRequest = makeShared<DnsRequest>();
    dnsRequest->setOperationCode(QUERY);
    dnsRequest->setRequestId(ctx.id);
    dnsRequest->appendQuestion(question);

    auto packet = new Packet("DNS Request");
    packet->insertAtBack(dnsRequest);

    // Send to client
    auto command = new DnsClientCommand();
    command->setRequestId(ctx.id);
    command->appendIpPool(ispResolver);
    command->setPayload(packet);

    send(command, "resolverOut");

    // Store as pending
    pendingRequests[ctx.id] = std::move(ctx);
}

void StubDnsResolver::handleResponse(DnsClientIndication *msg)
{
    // Get context
    auto iter = pendingRequests.find(msg->getRequestId());
    if (iter == pendingRequests.end())
        error("Unknown request id: %d", msg->getRequestId());

    // Process and generate response
    RequestContext &context = iter->second;
    std::set<ResourceRecord> records = processIndication(msg);
    sendResponse(context, records);
    pendingRequests.erase(iter);
    delete msg;
}

void StubDnsResolver::sendResponse(const RequestContext &context, std::set<ResourceRecord> records)
{
    StubDnsResponse *response = new StubDnsResponse();
    response->setDomain(context.request->getDomain());

    if (records.size())
    {
        response->setResult(0);
        response->setAddressArraySize(records.size());

        int i = 0;
        for (const auto &record : records)
            response->setAddress(i++, record.ip);
    }
    else
        response->setResult(1);

    // Send and release the context
    cModule *module = getSimulation()->getModule(context.request->getModuleId());
    sendDirect(response, module, "resolver");
}

std::set<ResourceRecord> StubDnsResolver::processIndication(DnsClientIndication *msg)
{
    std::set<ResourceRecord> records;
    if (msg->getResult() != OK)
        return records;

    auto packet = msg->getPayloadForUpdate();
    auto dnsResponse = packet->peekData<DnsRequest>();

    if (dnsResponse->getReturnCode() != ReturnCode::NOERROR)
        return records;

    if (dnsResponse->isAuthoritative())
        for (int i = 0; i < dnsResponse->getAuthoritativeAnswerArraySize(); i++)
            records.insert(dnsResponse->getAuthoritativeAnswer(i));
    else
        for (int i = 0; i < dnsResponse->getNonAuthoritativeAnswerArraySize(); i++)
            records.insert(dnsResponse->getNonAuthoritativeAnswer(i));

    auto &question = dnsResponse->getQuestion(0);
    for (const auto &record : records)
        dnsDatabase->insertRecord(question.getDomain(), record);

    return records;
}
