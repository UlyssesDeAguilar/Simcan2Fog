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
        auto iter = std::find_if(node->getRecords()->begin(), node->getRecords()->end(), [question](const ResourceRecord &rr)
                     { return matchWithWildcard(rr, question.getDomain()); });

        if (iter != node->getRecords()->end())
        {
            sendResponse(ctx, &(*iter));
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
    const ResourceRecord *record = processIndication(msg);
    sendResponse(context, record);
    pendingRequests.erase(iter);
    delete msg;
}

void StubDnsResolver::sendResponse(const RequestContext &context, const ResourceRecord *record)
{
    StubDnsResponse *response = new StubDnsResponse();
    response->setDomain(context.request->getDomain());

    if (record)
    {
        response->setResult(0);
        response->setAddress(record->ip);
    }
    else
        response->setResult(1);

    // Send and release the context
    cModule *module = getSimulation()->getModule(context.request->getModuleId());
    sendDirect(response, module, "resolver");
}

const ResourceRecord *StubDnsResolver::processIndication(DnsClientIndication *msg)
{
    const ResourceRecord *record = nullptr;
    if (msg->getResult() != OK)
        return nullptr;

    auto packet = msg->getPayloadForUpdate();
    auto dnsResponse = packet->peekData<DnsRequest>();

    if (dnsResponse->getReturnCode() != ReturnCode::NOERROR)
        return nullptr;

    if (dnsResponse->isAuthoritative())
        record = &dnsResponse->getAuthoritativeAnswer(0);
    else
        record = &dnsResponse->getNonAuthoritativeAnswer(0);

    auto &question = dnsResponse->getQuestion(0);
    dnsDatabase->insertRecord(question.getDomain(), *record);

    return record;
}
