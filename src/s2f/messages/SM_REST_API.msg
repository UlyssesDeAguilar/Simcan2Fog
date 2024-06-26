import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

// This is the base message for app intercommunication across the model
class SM_REST_API extends inet::FieldsChunk {
    //@customize(true);
    string url;         // Identifies the target service
    string payload;     // The payload (text encoded, probably should be JSON or XML)
}
