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

//
// Serialized block from a chain.
//
class GetBlockTxn extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    s2f::os::crypto::sha256digest blockHash;
    uint8_t index[];
};
