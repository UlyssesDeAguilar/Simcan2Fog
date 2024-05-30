import inet.common.packet.chunk.Chunk;

namespace iot;

enum IotCommand
{
    TURN_OFF = 0;
    TURN_ON = 1;
}

class IotPayload extends inet::FieldsChunk
{
    string tag;
    string value;
    string unit;
}