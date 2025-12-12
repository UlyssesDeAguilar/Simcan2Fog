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
class BlockTxn extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    s2f::os::crypto::sha256digest blockHash;
    Transaction tx[];
};
