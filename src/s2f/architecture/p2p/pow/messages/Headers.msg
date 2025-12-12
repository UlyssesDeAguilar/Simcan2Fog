//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

//
// Serialized block from a chain.
//
class SerializedBlockHeader extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    BlockHeader headers[];
};
