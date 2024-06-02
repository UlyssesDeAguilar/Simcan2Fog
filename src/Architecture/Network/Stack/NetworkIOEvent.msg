//import inet.common.packet.chunk.Chunk;
//import Messages.SIMCAN_Message;
import Core.PayloadMessage;

namespace networkio;

enum EventType
{
    SOCKET_PEER_CLOSED = 0;
    SOCKET_CLOSED = 1;
    SOCKET_FAILURE = 2;
    SOCKET_ESTABLISHED = 3;
    SOCKET_DATA_ARRIVED = 4;
    RESOLVER_NOERROR = 5;
    RESOLVER_NXDOMAIN = 6;
};

enum Command
{
    SOCKET_OPEN = 0;    // Request to open a socket, then the socketId is the port number
    SOCKET_CLOSE = 1;   // Request to close a socket
    SOCKET_SEND = 2;    // Request to send the data attached
    RESOLVE_DOMAIN = 3; // Request to resolve a given domain
};

message EventBase extends PayloadMessage
{
    uint32_t vmId;   // For the hypervisor
    uint32_t pid;
    uint32_t ip;
    uint32_t socketId;
    string serviceName;
}

message CommandEvent extends EventBase
{
    uint16_t command @enum(Command);    // Command
    uint32_t targetIp;
    uint32_t targetPort;
}

message IncomingEvent extends EventBase
{
    uint16_t type @enum(EventType);     // Type of the event
    uint32_t resolvedIp;                // Resolver
};