
//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus {{
  #include <s2f/architecture/p2p/pow/chain/Transaction.h>
}}

namespace s2f::p2p::pow;

class Transaction {  
    @existingClass;  
    @opaque;
}

//
// Serialized block from a chain.
//
class SerializedTxn extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    Transaction tx;
};
