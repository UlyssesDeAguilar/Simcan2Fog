//
// Bitcoin 'block' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#block
//
// Sends the class directly instead of a serialization.
// - Sent in response to 'getdata' with MSG_BLOCK
// - Sent unsolicited on mining.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

class SerializedBlock extends inet::FieldsChunk
{
    chunkLength = inet::B(1000);
    Block b;
};
