import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

//
// Message used to request blocks or headers
// They are essentially the same, but an 'inv' response to GetBlocks is 500 hashes at most
// And GetHeaders can be up to 2000
//
class GetBlocks extends inet::FieldsChunk
{
    chunkLength = inet::B(36);
    uint32_t version;  	                    // Protocol version, just like in 'version' messages
    s2f::os::crypto::sha256digest hashes[]; // Block hashes in local chain (newest to oldest) to find a common point
    s2f::os::crypto::sha256digest stopHash; // Last hash that should be sent. Defaults to 0 to receive ~500 hashes. 
};
