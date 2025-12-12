//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace s2f::p2p::pow;

//
// Serialized block from a chain.
//
class SendCmpct extends inet::FieldsChunk
{
    chunkLength = inet::B(9);
    bool announce;
    uint64_t version;
};
