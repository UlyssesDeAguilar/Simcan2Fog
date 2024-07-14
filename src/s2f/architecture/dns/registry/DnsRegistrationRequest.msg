import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

cplusplus{{
#include "s2f/architecture/dns/common.h"
}};

namespace dns;

// Will generate a descriptor for the struct (also reflexion for parallel simulation)
struct ResourceRecord{ @existingClass; @descriptor(false); }

//
// Binding of a domain to an IP address. It will be registered as an A record
//
struct DomainBinding
{
    ResourceRecord record;
    bool unregisterIfPresent = false;
}

//
// Request for binding a domain to an specified IP address
//
class DnsRegistrationRequest extends inet::FieldsChunk
{
	chunkLength = B(20);		// Packet length
	DomainBinding binding[];   // Domain bindings
}