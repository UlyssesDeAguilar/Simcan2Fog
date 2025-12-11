//
// Simplified block transmission message.
// Inspired on bitcoin, but moves the entire class instead of serializing.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus {{
  #include <s2f/architecture/p2p/pow/chain/Block.h>
}}

namespace s2f::p2p::pow;

class Block {  
    @existingClass;  
    @opaque;
}
//
// Serialized block from a chain.
//
class SerializedBlock extends inet::FieldsChunk
{
    chunkLength = inet::B(1000);
    Block b;
};
