import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

enum ProxyOperation
{
    REGISTER = 0;
    UNREGISTER = 1;
}

class ProxyServiceRequest extends inet::FieldsChunk 
{
    chunkLength = inet::B(100);
    uint32_t ip;
    uint32_t port;
    int socketId;
    string service;
    ProxyOperation operation;
}