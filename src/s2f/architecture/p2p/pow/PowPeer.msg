import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;
import s2f.apps.p2p.Peer;

namespace s2f::p2p::pow;

//
// Peer information for a PoW inspired protocol.
// 
// @author Tomás Daniel Expósito Torre
// @version 1.0
class PowPeer extends Peer
{
    uint32_t time;
    uint64_t services;
    uint16_t port;
};
