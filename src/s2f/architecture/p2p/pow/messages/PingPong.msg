//
// Bitcoin 'ping' and 'pong' messages.
// https://developer.bitcoin.org/reference/p2p_networking.html#ping
// https://developer.bitcoin.org/reference/p2p_networking.html#pong
//
// Used to keep a connection between two peers alive.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace s2f::p2p::pow;

class PingPong extends inet::FieldsChunk
{
	  chunkLength = inet::B(8); // Packet length
    uint64_t nonce;
}
