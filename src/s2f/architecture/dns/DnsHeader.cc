#include "s2f/architecture/dns/DnsHeader.h"

using namespace s2f::dns;

DnsHeader &DnsHeader::operator=(const DnsHeader &other)
{
    if (this == &other)
        return *this;
    DnsHeader_Base::operator=(other);
    copy(other);
    return *this;
}

void DnsHeader::setOperationCode(OperationCode code)
{
    uint8_t mask = code << 3;
    flag_field_1 = (flag_field_1 & (~0x78)) | mask;
}

OperationCode DnsHeader::getOperationCode() const
{
    uint8_t value = (flag_field_1 >> 3) & 0x0F;
    return static_cast<OperationCode>(value);
}

void DnsHeader::setResponseCode(ReturnCode code)
{
    flag_field_2 = (flag_field_2 & 0xF0) | code;
}

ReturnCode DnsHeader::getResponseCode() const
{
    uint8_t value = flag_field_2 & 0x0F;
    return static_cast<ReturnCode>(value);
}