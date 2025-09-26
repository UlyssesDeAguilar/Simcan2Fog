import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

namespace s2f::p2p;

struct NetworkPeer
{
    inet::L3Address ipAddress;
}

struct PowNetworkPeer extends NetworkPeer
{
    uint32_t time;
    uint64_t services;
    uint16_t port;
};

class PowMsgAddress extends inet::FieldsChunk
{
	  chunkLength = inet::B(20); // Packet length
    uint16_t ipAddressCount;
	  PowNetworkPeer ipAddresses[]; 
}
