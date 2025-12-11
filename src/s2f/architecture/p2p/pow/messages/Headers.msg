//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus {{
  #include <s2f/architecture/p2p/pow/chain/Block.h>
}}

namespace s2f::p2p::pow;

struct BlockHeader {  
    @existingClass;  
    @opaque;
}

//
// Serialized block from a chain.
//
class SerializedBlockHeader extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    BlockHeader headers[];
};
