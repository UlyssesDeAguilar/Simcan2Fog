import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace s2f::p2p;

//
// Header inspired in the bitcoin p2p message header
//
class Header extends inet::FieldsChunk
{
    chunkLength = inet::B(32);
    uint32_t startString;  	// Magic number indicating origin network 
    string commandName; 		// Message type that is null-padded (0x00)
    uint32_t payloadSize; 		// Number of bytes, maxed to 32MiB
    string checksum;     		// First 4 bytes of SHA256(SHA256(payload))
};
