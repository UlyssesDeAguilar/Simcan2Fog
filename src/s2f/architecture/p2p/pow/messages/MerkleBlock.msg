//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus {{
  #include <s2f/architecture/p2p/pow/chain/Block.h>
  #include <s2f/os/crypto/hash.h>
}}

class s2f::os::crypto::sha256digest {  
    @existingClass;  
    @opaque;
}


namespace s2f::p2p::pow;

struct BlockHeader {  
    @existingClass;  
    @opaque;
}

//
// Serialized block from a chain.
//
class MerkleBlock extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    BlockHeader header;

    s2f::os::crypto::sha256digest hashes[]; 
    uint8_t flags[];
};
