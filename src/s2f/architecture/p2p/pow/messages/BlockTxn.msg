//
// Bitcoin 'blocktxn' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#blocktxn
//
// Used in tandem with 'cmpctblock'
// - Sent in response to 'getblocktxn'
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

class BlockTxn extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    s2f::os::crypto::sha256digest blockHash;
    Transaction tx[];
};
