import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.architecture.dns.ResourceRecord;

namespace dns;

enum OperationCode
{
    QUERY  = 0;  //!< Standard query
    IQUERY = 1;  //!< Used for inverse querys 				(obsolete)
    STATUS = 2;  //!< Request for status					(currently not used)
    NOTIFY = 3;  //!< Notify for Zone Transfer				(currently not used)
};

enum ReturnCode
{
    NOERROR  = 0;  //!< DNS Query completed successfully
    FORMERR  = 1;  //!< DNS Query Format Error
    SERVFAIL = 2;  //!< Server failed to complete the DNS request
    NXDOMAIN = 3;  //!< Domain name does not exist
    NOTIMP	 = 4;  //!< Function not implemented
    REFUSED  = 5;  //!< The server refused to answer for the query (currently not used)
    YXDOMAIN = 6;  //!< Name that should not exist, does exist	  (currently not used)
    XRRSET   = 7;  //!< RRset that should not exist, does exist	  (currently not used)
    NOTAUTH  = 8;  //!< Server not authoritative for the zone	  (currently not used)
    NOTZONE  = 9;  //!< Name not in zone							  (currently not used)
};

class DnsQuestion
{
    string domain;
    RecordType questionType;
};

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
