//
// Simplified BlockHeader transmission message.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

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
