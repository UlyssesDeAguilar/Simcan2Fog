import Messages.SIMCAN_Message;

namespace networkio;

enum EventType
{
    SOCKET_PEER_CLOSED = 0;
    SOCKET_CLOSED = 1;
    SOCKET_FAILURE = 2;
    SOCKET_ESTABLISHED = 3;
};

enum Command
{
    SOCKET_OPEN = 0;    // Request to open a socket, then the socketId is the port number
    SOCKET_CLOSE = 1;   // Request to close a socket
    SOCKET_SEND = 2;    // Request to send the data attached
};


// rips - SOCKET_OPEN
//      For the services it's the IP where they will serve
//      For the clients it's the remote IP where they'll connect to
//
// rips - Otherwise
//      It refers to the "real" socketId
//
message Event{
    uint32_t vmId;   // For the hypervisor
    uint32_t pid;
    
    uint32_t rips;                      // Ip/Socket Register 
    uint16_t type @enum(EventType);     // Type of the event
    uint16_t command @enum(Command);    // Command
    string serviceName;                 // When registering a service, the domain to be binded
    SIMCAN_Message *package;            // When responding/requesting
};