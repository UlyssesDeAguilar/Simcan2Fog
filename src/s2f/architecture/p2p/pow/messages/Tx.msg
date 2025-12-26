
//
// Bitcoin 'tx' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#tx
//
// - Sent in response to 'getdata' with MSG_TX
// - Sent in response to 'getdata' with MSG_MERKLEBLOCK
// - Sent unsolicited.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

class SerializedTxn extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    Transaction tx;
};
