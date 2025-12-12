//
// Bitcoin 'getblocktxn' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#getblocktxn
//
// - Sent on demand
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

class GetBlockTxn extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    s2f::os::crypto::sha256digest blockHash;
    uint8_t index[];
};
