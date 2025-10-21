import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;


//
// Version message payload 
//
class PowMsgVersion extends inet::FieldsChunk
{
    chunkLength = inet::B(90);    // TODO: change dummy size

    int32_t version;              // Highest protocol understood by node
    uint64_t services;            // Services supported by transmitting node
    int64_t timestamp;            // Unix epoch time

    // Receiving node
    uint64_t addrRecvServices;    // Supported services
    inet::L3Address addrRecvIpAddress;     // IPv6 address: IPv4 supported as IPv4-mapped IPv6
    uint16_t addrRecvPort;        // Port number

    // Transmitting node
    uint64_t addrTransServices;   // Supported services
    inet::L3Address addrTransIpAddress;    // IPv6 address: IPv4 supported as IPv4-mapped IPv6
    uint16_t addrTransPort;       // Port number

    uint64_t nonce;               // Random nonce that helps detect self-connections
    uint64_t userAgentBytes;    // Size of user_agent field
    string userAgent;            // User Agent

    int32_t startHeight;         // Height of transmitting best block chain
    bool relay;                   // Optional field, blocks new transactions on false
};
