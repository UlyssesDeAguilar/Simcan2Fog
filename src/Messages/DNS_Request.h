#include "Messages/DNS_Request_m.h"

namespace dns
{
    class DNS_Request : public DNS_Request_Base
    {
    private:
        void copy(const DNS_Request& other) { this->chunkLength = other.chunkLength; }
    public:
        // make constructors protected to avoid instantiation
        DNS_Request();
        DNS_Request(const DNS_Request& other) : DNS_Request_Base(other) {copy(other);}
        // make assignment operator protected to force the user override it
        DNS_Request &operator=(const DNS_Request &other);
    };
}