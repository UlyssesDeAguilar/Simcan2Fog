#ifndef SIMCAN_EX_DNS_RESOLVER
#define SIMCAN_EX_DNS_RESOLVER

#include "s2f/architecture/dns/DnsCommon.h"
#include "s2f/architecture/dns/db/DnsDb.h"
#include "s2f/architecture/dns/client/DnsClientCommand_m.h"
#include "s2f/architecture/net/stack/resolver/StubDnsRequest_m.h"

class StubDnsResolver : public omnetpp::cSimpleModule
{

protected:
    struct RequestContext
    {
        uint16_t id;
        StubDnsRequest *request{};
        
        RequestContext() : id(0), request(nullptr) {}
        RequestContext(uint16_t id, StubDnsRequest *request) : id(id), request(request) {}
        RequestContext(const RequestContext &other) noexcept : id(other.id), request(other.request->dup()) {}
        RequestContext(RequestContext &&other) noexcept : id(other.id), request(other.request) { other.request = nullptr; }
        
        RequestContext &operator=(RequestContext &&other) noexcept
        {
            id = other.id;
            request = other.request;
            other.request = nullptr;
            return *this;
        }
        
        RequestContext &operator=(const RequestContext &other) noexcept
        {
            id = other.id;
            request = other.request->dup();
            return *this;
        }

        ~RequestContext() { delete request; }
    };

    std::map<uint16_t, RequestContext> pendingRequests;
    inet::L3Address ispResolver;
    dns::DnsDb *dnsDatabase;
    uint16_t lastId{}; // Last reserved id (gives linearity and better likelyhood of finding a free one)

    // Kernel lifecycle
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;

    void handleRequest(StubDnsRequest *request);
    void handleResponse(dns::DnsClientIndication *msg);
    const dns::ResourceRecord *processIndication(dns::DnsClientIndication *msg);
    void sendResponse(const RequestContext &context, const dns::ResourceRecord *record);
};

#endif
