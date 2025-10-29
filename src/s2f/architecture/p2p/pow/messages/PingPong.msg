
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace s2f::p2p;

class PingPong extends inet::FieldsChunk
{
	  chunkLength = inet::B(8); // Packet length
    uint64_t nonce;
}
