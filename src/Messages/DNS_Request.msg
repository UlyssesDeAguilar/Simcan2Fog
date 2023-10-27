import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
//namespace inet;

cplusplus {{

/* 
	Based on the RFC 1034/1035 and the explanation found in:
	http://www.tcpipguide.com/free/t_DNSNameServerDataStorageResourceRecordsandClasses-3.htm#Table_166
*/
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/chunk/FieldsChunk.h"
using namespace inet;

const B HEADER_BYTES = B(12);

typedef enum{
	A,			// Address
	NS,			// Name Server
	CNAME,		// Canonical Name
	SOA,		// Start of Authority
	PTR,		// Pointer
	MX,			// eMail eXchange
	TXT			// Text string (arbitrary text)
} RR_Type;

struct ReturnRecord {
	RR_Type type;
	inet::L3Address ip;
	std::string url;
};

}}

//class FieldsChunk;
struct ReturnRecord{ @existingClass; }
class FieldsChunk{ @existingClass; }

//namespace dns;
class DNS_Request extends FieldsChunk{
	uint16_t requestId;		// Used by the client to correlate request <-> response
	uint8_t operationCode;
	uint8_t returnCode;
	//uint16_t questionCount;
	//uint16_t answerCount;
	string questions[];
	ReturnRecord responses[];
}
