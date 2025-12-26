//
// Bitcoin 'sendcmpct' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#sendcmpct
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace s2f::p2p::pow;

class SendCmpct extends inet::FieldsChunk
{
    chunkLength = inet::B(9);
    bool announce;
    uint64_t version;
};
