import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

// Declared above to not be included on the namespace scope
cplusplus {{
/* 
	Based on the RFC 1034/1035 and the explanation found in:
	http://www.tcpipguide.com/free/t_DNSNameServerDataStorageResourceRecordsandClasses-3.htm#Table_166
*/
#include "inet/networklayer/common/L3Address.h"
#include "inet/common/packet/chunk/FieldsChunk.h"
using namespace inet;
}}

// We don't want to generate a descriptor for this class
class FieldsChunk{ @existingClass; @descriptor(false);}

namespace dns;

// Declared below to be included on the namespace scope
cplusplus {{
const B HEADER_BYTES = B(12);

typedef enum{
	QUERY,		// Standard query
	IQUERY,		// Used for inverse querys 				(obsolete)
	STATUS,		// Request for status					(currently not used)
	NOTIFY,		// Notify for Zone Transfer				(currently not used)
	UPDATE_R,	// Update the Resource Records (Add new ones)
	UPDATE_D	// Update the Resource Records (Delete selected ones)
} OP_Code;

typedef enum{
	NOERROR, 	// DNS Query completed successfully
	FORMERR, 	// DNS Query Format Error
	SERVFAIL, 	// Server failed to complete the DNS request
	NXDOMAIN, 	// Domain name does not exist
	NOTIMP, 	// Function not implemented
	REFUSED, 	// The server refused to answer for the query (currently not used)
	YXDOMAIN, 	// Name that should not exist, does exist	  (currently not used)
	XRRSET, 	// RRset that should not exist, does exist	  (currently not used)
	NOTAUTH, 	// Server not authoritative for the zone	  (currently not used)
	NOTZONE 	// Name not in zone							  (currently not used)
} ReturnCode;

typedef enum{
	A,			// Address
	NS,			// Name Server
	CNAME,		// Canonical Name
	SOA,		// Start of Authority
	PTR,		// Pointer
	MX,			// eMail eXchange
	TXT			// Text string (arbitrary text)
} RR_Type;

struct ResourceRecord {
	RR_Type type;
	inet::L3Address ip;
	std::string url;

	const char *typeToStr() const{
		const char* RR_TypeToStr[] = {"A","NS","CNAME","SOA","PTR","MX","TXT"} ;
		return RR_TypeToStr[type];
	}
};
}}


// Will generate a descriptor for the struct (also reflexion for parallel simulation)
struct ResourceRecord{ @existingClass; }


//namespace dns;
class DNS_Request extends FieldsChunk{
	uint16_t requestId;		// Used by the client to correlate request <-> response
	uint8_t operationCode;
	uint8_t returnCode;
	//uint16_t questionCount;
	//uint16_t answerCount;
	string question[];
	ResourceRecord record[];
}
