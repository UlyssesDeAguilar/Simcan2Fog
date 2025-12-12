import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import MsgMetadata;

namespace s2f::p2p::pow;

enum InventoryIdentifier
{
    MSG_TX = 0;     // TXID hash
    MSG_BLOCK = 1;  // Block header hash
    MSG_FILTERED_BLOCK = 2; // Identical to MSGBLOCK. When used in getdata, indicates 'merkleblock' response
    MSG_CMPCT_BLOCK = 3; // Identical to MSGBLOCK. When used in getdata, indicates 'cmpctblock' response
    MSG_WITNESS_TX = 4; // Identical to MSGBLOCK. When used in getdata, indicates the response is block with transactions that has witness.
};


//
// Inventory datatype used to send transactions and blocks
//
class Inventory extends inet::FieldsChunk
{
    chunkLength = inet::B(36);
    uint32_t identifier;  	// Type of object hashed
    s2f::os::crypto::sha256digest hash;      // SHA256(SHA256()) hash of the object
};
