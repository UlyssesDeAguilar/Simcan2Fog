//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus {{
  #include <s2f/architecture/p2p/pow/chain/Block.h>
  #include <s2f/architecture/p2p/pow/chain/Transaction.h>
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

class Transaction {  
    @existingClass;  
    @opaque;
}

struct PrefilledTransaction {
    uint8_t index;
    Transaction tx;
}

//
// Serialized block from a chain.
//
class CmpctBlock extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    BlockHeader header;
    uint8_t nonce;
    uint8_t shortId[];
    PrefilledTransaction prefilledTxn[];
};
