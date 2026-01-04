import inet.common.packet.chunk.Chunk;

namespace s2f::iot;

enum PowerState
{
    OFF = 0;
    ON = 1;
}

message IotPowerEvent
{
    PowerState powerStateToTransition;
}

class IotCommand extends inet::FieldsChunk
{
    PowerState powerState;
}

class IotPayload extends inet::FieldsChunk
{
    string tag;
    double value;
    string unit;
}