import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

enum StartString
{
    MAIN_NET = 0xF9BEB4D9;
    TEST_NET = 0x0B110907;
    REG_TEST = 0xFABFB5DA; 
}

//
// Header inspired in the bitcoin p2p message header
//
class PowMsgHeader extends inet::FieldsChunk
{
    chunkLength = inet::B(32);
    StartString startString;  	// Magic number indicating origin network 
    string commandName; 		// Message type that is null-padded (0x00)
    uint32_t payloadSize; 		// Number of bytes, maxed to 32MiB
    string checksum;     		// First 4 bytes of SHA256(SHA256(payload))
};
