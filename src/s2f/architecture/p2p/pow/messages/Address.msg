//
// Bitcoin 'addr' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#addr
//
// Relays connection information for peers.
// - Sent in response to 'getaddr'
// - Sent unsolicited
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;
import s2f.architecture.p2p.pow.PowPeer;

namespace s2f::p2p::pow;

class Address extends inet::FieldsChunk
{
	  chunkLength = inet::B(20); // Packet length
	  PowPeer *ipAddress[]; 
}
