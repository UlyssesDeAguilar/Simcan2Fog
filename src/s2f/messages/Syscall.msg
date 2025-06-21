import s2f.messages.SIMCAN_Message;
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace s2f::os;

//
// System call codes.
// These are the identifiers of the operation to be done
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum SyscallCode
{
    READ = 0; // Read from disk
    WRITE = 1; // Write to disk
    EXEC = 2; // Execute / Calculate
    OPEN_CLI = 3; // Open client socket : PORT/IP
    OPEN_SERV = 4; // Open server socket : PORT/DOMAIN/SERVICE_NAME
    RESOLVE = 5; // Resolve a fully qualified DNS domain name
    SEND = 6; // Send through socket
    RECV = 7; // Recieve through socket
    CLOSE = 8; // Close a socket
    EXIT = 9; // Finish the process (gracefully)
    ABORT = 10; // Finish the process (abruptly)
};

//
// Systemcall message.
// It carries over the information for the request/response
// between the apps and the OS components.
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
message Syscall extends SIMCAN_Message {
    operation = SM_Syscall_Req; // SIMCAN request type 
    int pid; // Process ID
    int vmId; // VM ID
    uint16_t opCode @enum(SyscallCode); // Syscall code
}

//
// Type of socket
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum ConnectionMode
{
    SOCK_DGRAM = 0; // For UDP traffic
    SOCK_STREAM = 1; // For TCP traffic
};

//
// Specialization of the Systemcall applied to the disk operations.
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
message DiskSyscall extends Syscall
{
    double bufferSize; // Size of the buffer to be written or read
    bool abortIOs = false; // Wheter to abort all the IO operations of the VM/App
}