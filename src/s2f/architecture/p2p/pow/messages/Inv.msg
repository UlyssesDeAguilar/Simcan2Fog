//
// Bitcoin 'inv' and 'getdata' message.
// https://developer.bitcoin.org/reference/p2p_networking.html#inv
// https://developer.bitcoin.org/reference/p2p_networking.html#getdata
//
// - Sent in response to 'getblocks'
// - Sent in response to 'mempool'
// - 'getdata' is just 'inv' with a changed header (B -> inv -> A -> getdata -> B,C,...)
//
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

import Inventory;

namespace s2f::p2p::pow;

class Inv extends inet::FieldsChunk
{
    chunkLength = inet::B(20); // Packet length
    Inventory inventory[];
}
