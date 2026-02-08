#include "s2f/architecture/dns/RecordData.h"

using namespace s2f::dns;
using namespace omnetpp;
using namespace inet;

RecordData::RecordData(const L3Address &address)
{
    if (address.getType() != L3Address::IPv4 && address.getType() != L3Address::IPv6)
        throw cRuntimeError("dns::RecordData: Only IP addresses are supported from L3Address");

    resourceDataLength = address.getType() == L3Address::IPv4 ? 4 : 16;
    memoryType = DataMemoryType::ADDR;
    resourceData = DataField(new uint8_t[resourceDataLength], std::default_delete<uint8_t[]>());

    if (address.getType() == L3Address::IPv4)
    {
        uint32_t ipv4 = address.toIpv4().getInt();
        memcpy(resourceData.get(), &(ipv4), resourceDataLength);
    }
    else
    {
        Ipv6Address ipv6 = address.toIpv6();
        memcpy(resourceData.get(), ipv6.words(), resourceDataLength);
    }
}

L3Address RecordData::getDataAsAddress() const
{
    L3Address address;

    if (memoryType != DataMemoryType::ADDR)
        throw cRuntimeError("dns::RecordData: Invalid conversion from internal memory type to address");

    uint32_t *word_pointer = reinterpret_cast<uint32_t *>(this->resourceData.get());

    if (resourceDataLength == 4)
        address = Ipv4Address(*word_pointer);
    else
        address = Ipv6Address(word_pointer[0], word_pointer[1], word_pointer[2], word_pointer[3]);

    return address;
}

RecordData::RecordData(DnsName name)
{
    resourceDataLength = name.getLength();
    resourceData = name.data;
    memoryType = DataMemoryType::DNS_NAME;
}

DnsName RecordData::getDataAsDnsName() const
{
    if (memoryType != DataMemoryType::DNS_NAME)
        throw cRuntimeError("dns::RecordData: Invalid conversion from internal memory type to DNS name");
    
    DnsName name(*this);
    return name;
}


RecordData::RecordData(const char *text, size_t length)
{
    if (length > std::numeric_limits<uint16_t>::max())
        throw cRuntimeError("dns::RecordData: Text too long, the maximum length is %d", std::numeric_limits<uint16_t>::max());

    resourceDataLength = length;
    resourceData = DataField(new uint8_t[length], std::default_delete<uint8_t[]>());
    memcpy(resourceData.get(), text, length);
    memoryType = DataMemoryType::TEXT;
}

opp_string RecordData::getDataAsText() const
{
    if (memoryType != DataMemoryType::TEXT)
        throw cRuntimeError("dns::RecordData: Invalid conversion from internal memory type to text");

    return opp_string(reinterpret_cast<char*>(resourceData.get()), resourceDataLength);
}

uint8_t RecordData::getResourceDataByte(uint16_t index) const
{
    if (index >= resourceDataLength)
        throw cRuntimeError("dns::RecordData: Invalid index: %d", index);
    return resourceData[index];
}

RecordData &RecordData::operator=(const L3Address &address)
{
    RecordData data(address);
    *this = data;
    return *this;
}

RecordData &RecordData::operator=(const DnsName &name)
{
    RecordData data(name);
    *this = data;
    return *this;
}