import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

class NodeEvent extends inet::FieldsChunk
{
    string nodeTopic;
    uint64_t availableCores;
}
