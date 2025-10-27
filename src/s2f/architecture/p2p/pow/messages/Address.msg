import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;
import s2f.architecture.p2p.pow.PowNetworkPeer;

namespace s2f::p2p;

class Address extends inet::FieldsChunk
{
	  chunkLength = inet::B(20); // Packet length
    uint16_t ipAddressCount;
	  PowNetworkPeer *ipAddress[]; 
}
