import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

class RestMsg extends inet::FieldsChunk 
{
    string hostname;    // Identifies the host of the service (typically from a domain name)
}