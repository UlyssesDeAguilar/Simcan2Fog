import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;
import s2f.apps.p2p.NetworkPeer;

namespace s2f::p2p;

//
// State connection for a peer.
// 
// @author Tomás Daniel Expósito Torre
// @version 1.0
enum ConnectionState
{
    CANDIDATE = 0;
    CONNECTED = 1;
};

//
// Peer information for a PoW inspired protocol.
// 
// @author Tomás Daniel Expósito Torre
// @version 1.0
class PowNetworkPeer extends NetworkPeer
{
    ConnectionState state;
    uint32_t time;
    uint64_t services;
    uint16_t port;
};
