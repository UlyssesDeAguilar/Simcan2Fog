import s2f.messages.SIMCAN_Message;
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace hypervisor;

cplusplus{{
    #include "inet/common/Ptr.h"
    using PTR = inet::Ptr<const inet::Chunk>;
}}

class PTR {@existingClass; @descriptor(false);}


enum SyscallCode
{
    READ = 0;      // Read from disk
    WRITE = 1;     // Write to disk
    EXEC = 2;      // Execute / Calculate
    OPEN_CLI = 3;  // Open client socket : PORT/IP
    OPEN_SERV = 4; // Open server socket : PORT/DOMAIN/SERVICE_NAME
    RESOLVE = 5;   // Resolve a fully qualified DNS domain name
    SEND = 6;      // Send through socket
    RECV = 7;      // Recieve through socket
    CLOSE = 8;     // Close a socket
    EXIT = 9;      // Finish the process (gracefully)
    ABORT = 10;    // Finish the process (abruptly)
};

enum ConnectionMode
{
    SOCK_DGRAM = 0;
    SOCK_STREAM = 1;
};

message Syscall extends SIMCAN_Message {
    operation = SM_Syscall_Req;     // SIMCAN request type 
    int pid;                        // Process ID
    int vmId;                       // VM ID
    uint16_t opCode @enum(SyscallCode);
}

message DiskSyscall extends Syscall
{
    double bufferSize;
}

message ResolverSyscall extends Syscall
{
    opCode = RESOLVE;
    string domainName;
    uint32_t resolvedIp;
}

message SocketIoSyscall extends Syscall
{
    int socketFd;
    uint32_t targetIp;
    uint32_t targetPort;
    ConnectionMode mode;
    string serviceName;
    PTR payload;
}