import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

namespace s2f::p2p;

struct NetworkPeer
{
    uint32_t time;
    uint64_t services;
    inet::L3Address ipAddress;
    uint16_t port;
};

class PowMsgAddress extends inet::FieldsChunk
{
	chunkLength = inet::B(20); // Packet length
  	uint16_t ipAddressCount;
	NetworkPeer ipAddresses[]; 
}
