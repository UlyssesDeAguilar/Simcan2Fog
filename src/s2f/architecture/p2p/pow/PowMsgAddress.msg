import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;

namespace s2f::p2p;

enum ConnectionState
{
    CANDIDATE = 0;
    CONNECTED = 1;
};

class NetworkPeer
{
    inet::L3Address ipAddress;
};

class PowNetworkPeer extends NetworkPeer
{
    ConnectionState state;
    uint32_t time;
    uint64_t services;
    uint16_t port;
};

class PowMsgAddress extends inet::FieldsChunk
{
	  chunkLength = inet::B(20); // Packet length
    uint16_t ipAddressCount;
	  PowNetworkPeer *ipAddress[]; 
}
