import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.architecture.dns.DnsDefinitions;

namespace s2f::dns;

//
// DNS Protocol Header based on RFC 1034/1035
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
class DnsHeader extends inet::FieldsChunk
{
    @customize(true);
	chunkLength = inet::B(DNS_HEADER_SIZE);	// Chunk length
	uint16_t id; // Identifier of the transaction
	uint8_t flag_field_1; // Includes the flags: QR/OpCode/AA/TC/RD
	uint8_t flag_field_2; // Includes the flags: RA/Z/RCode
    uint16_t questionCount; // Number of questions
    uint16_t answerCount; // Number of answers
    uint16_t authorityRecordCount; // Number of records in the Authority section
    uint16_t additionalRecordCount; // Number of records in the Additional section
}