#include "s2f/architecture/dns/ResourceRecord.h"

using namespace omnetpp;
using namespace s2f::dns;
using namespace inet;

ResourceRecord::ResourceRecord(const DnsName &domain, const L3Address &address, uint32_t ttl)
{
    name = domain;
    dnsClass = INTERNET;
    type = address.getType() == L3Address::IPv4 ? RRType::A : RRType::AAAA;
    this->ttl = ttl;
    data = RecordData(address);
}

ResourceRecord::ResourceRecord(const DnsName &canonical_name, const DnsName &domain, uint32_t ttl)
{
    name = domain;
    dnsClass = INTERNET;
    type = RRType::CNAME;
    this->ttl = ttl;
    data = RecordData(canonical_name);
}

ResourceRecord::ResourceRecord(const DnsName &domain, const char *text, size_t text_length, uint32_t ttl)
{
    name = domain;
    dnsClass = INTERNET;
    type = RRType::TXT;
    this->ttl = ttl;
    data = RecordData(text, text_length);
}

size_t ResourceRecord::getSize() const
{
    return name.getLength() +
           RR_TYPE_SIZE +
           RR_CLASS_SIZE +
           RR_TTL_SIZE +
           RR_DATA_LEN_SIZE +
           data.getResourceDataLength();
}
