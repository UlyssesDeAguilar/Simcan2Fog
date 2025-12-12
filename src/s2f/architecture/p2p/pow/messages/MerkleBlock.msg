//
// Bitcoin 'merkleblock' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#merkleblock
//
// - Sent in response to 'getdata' with MSG_MERKLEBLOCK flags.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

class MerkleBlock extends inet::FieldsChunk
{
    chunkLength = inet::B(80);
    BlockHeader header;

    s2f::os::crypto::sha256digest hashes[]; 
    uint8_t flags[];
};
