import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
cplusplus{{
#include "Architecture/Network/DNS/common.h"
}};

namespace dns;

// Will generate a descriptor for the struct (also reflexion for parallel simulation)
struct ResourceRecord{ @existingClass; }

class DnsRequest extends inet::FieldsChunk
{
	chunkLength = 20;		// Header length
	uint16_t requestId;		// Used by the client to correlate request <-> response
	uint8_t operationCode;
	uint8_t returnCode; 
	bool isAuthoritative;	// Wheter the answer is from an authoritative name server
	string question[];
	ResourceRecord nonAuthoritativeAnswers[];
	ResourceRecord authoritativeAnswers[];
}
