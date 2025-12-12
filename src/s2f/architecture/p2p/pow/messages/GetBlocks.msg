//
// Bitcoin 'getblocks' and 'getheaders' messages.
// https://developer.bitcoin.org/reference/p2p_networking.html#getblocks
// https://developer.bitcoin.org/reference/p2p_networking.html#getheaders
//
// 'getblocks' requests an 'inv' message with header hashes from a certain point in the chain.
// - Sent on demand
// - 500 hashes at most
// - Can specify limit hash if the node knows it
//
// 'getheaders' requests a 'headers' message.
// - Sent on demand
// - 2000 hashes at most.
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

class GetBlocks extends inet::FieldsChunk
{
    chunkLength = inet::B(36);
    uint32_t version;  	                    // Protocol version, just like in 'version' messages
    s2f::os::crypto::sha256digest hashes[]; // Block hashes in local chain (newest to oldest) to find a common point
    s2f::os::crypto::sha256digest stopHash; // Last hash that should be sent. Defaults to 0 for no-stop.
};
