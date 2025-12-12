//
// Bitcoin 'headers' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#headers
//
// - Sent in response to 'getheaders'
// - Can be empty
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

class SerializedBlockHeader extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    BlockHeader headers[];
};
