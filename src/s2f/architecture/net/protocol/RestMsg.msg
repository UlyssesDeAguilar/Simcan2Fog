import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

//
//  Request inspired on HTTP v1.1 protocol 
//
class RestMsg extends inet::FieldsChunk
{
    chunkLength = inet::B(100);
    string host;            // FQDN/Ip of the service trying to be reached 
    string path;            // Path to the resource
};