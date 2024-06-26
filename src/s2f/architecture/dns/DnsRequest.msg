import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus{{
#include "s2f/architecture/dns/common.h"
}};

namespace dns;

// Will generate a descriptor for the struct (also reflexion for parallel simulation)
struct ResourceRecord{ @existingClass; }

class DnsRequest extends inet::FieldsChunk
{
	chunkLength = B(20);		// Header length
	uint16_t requestId;		// Used by the client to correlate request <-> response
	uint8_t operationCode;
	uint8_t returnCode; 
	bool isAuthoritative;	// Wheter the answer is from an authoritative name server
	string question[];
	ResourceRecord nonAuthoritativeAnswers[];
	ResourceRecord authoritativeAnswers[];
}
