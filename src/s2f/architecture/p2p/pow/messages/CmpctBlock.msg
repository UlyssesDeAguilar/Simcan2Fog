//
// Bitcoin 'cmpctblock' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#cmpctblock
//
// - Sent in response to 'getdata' with MSG_CMPCT_BLOCK.
// - Sent in response to 'sendcmpct' if the block is recent enough.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

struct PrefilledTransaction {
    uint8_t index;
    Transaction tx;
}

class CmpctBlock extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    BlockHeader header;
    uint8_t nonce;
    uint8_t shortId[];
    PrefilledTransaction prefilledTxn[];
};
