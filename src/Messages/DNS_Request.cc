#include "DNS_Request.h"
using namespace dns;

DNS_Request::DNS_Request()
{
    chunkLength = HEADER_BYTES;
}

DNS_Request &DNS_Request::operator=(const DNS_Request &other)
{
    DNS_Request_Base::operator=(other);
    copy(other);
    return *this;
}