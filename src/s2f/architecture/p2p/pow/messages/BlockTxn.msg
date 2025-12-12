//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus {{
  #include <s2f/architecture/p2p/pow/chain/Transaction.h>
  #include <s2f/os/crypto/hash.h>
}}

class s2f::os::crypto::sha256digest {  
    @existingClass;  
    @opaque;
}


namespace s2f::p2p::pow;

class Transaction {  
    @existingClass;  
    @opaque;
}

//
// Serialized block from a chain.
//
class BlockTxn extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    s2f::os::crypto::sha256digest blockHash;
    Transaction tx[];
};
