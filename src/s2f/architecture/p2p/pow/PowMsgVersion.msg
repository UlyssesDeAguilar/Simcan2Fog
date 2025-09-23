import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

enum PowServiceType
{
  UNNAMED = 0x00; // Not a full node, may only be ble to provide transactions
  NODE_NETWORK = 0x01; // Full node
  NODE_GETUTXO = 0x02; // Full node with getutxo support
  NODE_BLOOM = 0x04;   // Full node with bloom-filtered connections
  NODE_WITNESS = 0x08; // Full node with support for witness data in blocks
  NODE_XTHIN = 0x10;   // Full node with support for Xtreme Thinblocks.
  NODE_NETWORK_LIMITED = 0x0400; // Same as 0x01 with restrictions from BIP159
};

//
// Version message payload 
//
class PowMsgVersion extends inet::FieldsChunk
{
    chunkLength = inet::B(90);    // TODO: change dummy size

    int32_t version;              // Highest protocol understood by node
    PowServiceType services;            // Services supported by transmitting node
    int64_t timestamp;            // Unix epoch time

    // Receiving node
    PowServiceType addrRecvServices;    // Supported services
    string addrRecvIpAddress;     // IPv6 address: IPv4 supported as IPv4-mapped IPv6
    uint16_t addrRecvPort;        // Port number

    // Transmitting node
    PowServiceType addrTransServices;   // Supported services
    string addrTransIpAddress;    // IPv6 address: IPv4 supported as IPv4-mapped IPv6
    uint16_t addrTransPort;       // Port number

    uint64_t nonce;               // Random nonce that helps detect self-connections
    uint64_t userAgentBytes;    // Size of user_agent field
    string userAgent;            // User Agent

    int32_t startHeight;         // Height of transmitting best block chain
    bool relay;                   // Optional field, blocks new transactions on false
};
