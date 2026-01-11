import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.architecture.dns.ResourceRecord;
import s2f.architecture.dns.DnsDefinitions;

namespace s2f::dns;

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
