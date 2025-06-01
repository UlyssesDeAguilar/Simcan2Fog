import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.architecture.dns.ResourceRecord;

namespace s2f::dns;

//
// DNS header field for operation codes
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum OperationCode
{
    QUERY  = 0;  // Standard query
    IQUERY = 1;  // Used for inverse querys 				(obsolete)
    STATUS = 2;  // Request for status					(currently not used)
    NOTIFY = 3;  // Notify for Zone Transfer				(currently not used)
};

//
// DNS header field for return codes
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum ReturnCode
{
    NOERROR  = 0;  // DNS Query completed successfully
    FORMERR  = 1;  // DNS Query Format Error
    SERVFAIL = 2;  // Server failed to complete the DNS request
    NXDOMAIN = 3;  // Domain name does not exist
    NOTIMP	 = 4;  // Function not implemented
    REFUSED  = 5;  // The server refused to answer for the query (currently not used)
    YXDOMAIN = 6;  // Name that should not exist, does exist	  (currently not used)
    XRRSET   = 7;  // RRset that should not exist, does exist	  (currently not used)
    NOTAUTH  = 8;  // Server not authoritative for the zone	  (currently not used)
    NOTZONE  = 9;  // Name not in zone							  (currently not used)
};

//
// DNS question
// It represents a query for a domain and a record type
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
class DnsQuestion
{
    string domain; // Domain which is the subject of the question
    RecordType questionType; // The record type for the expected result
};

//
// DNS request
// It represents both a DNS query and response as per RFC 1034/1035
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
class DnsRequest extends inet::FieldsChunk
{
	chunkLength = inet::B(20);							// Chunk length
	uint16_t requestId;									// Used by the client to correlate request <-> response
	uint8_t operationCode @enum(OperationCode);			// Operation Code
	uint8_t returnCode @enum(ReturnCode); 				// Return Code
	bool isAuthoritative;								// Wheter the answer is from an authoritative name server
	DnsQuestion question[];								// Questions
	ResourceRecord nonAuthoritativeAnswer[];	        // Non authoritative answers
	ResourceRecord authoritativeAnswer[];	        	// Authoritative answers
}
