import inet.common.packet.chunk.Chunk;

namespace s2f::iot;

//
// Power state enumeration
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum PowerState
{
    OFF = 0; // Device is off
    ON = 1; // Device is on
}

//
// Power event. It's used for delay on the ON/OFF operations
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
message IotPowerEvent
{
    PowerState powerStateToTransition; // Power state to transition to
}

//
// Iot Command
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
class IotCommand extends inet::FieldsChunk
{
    PowerState powerState; // Power state command for the sensor
}

//
// Iot Sensor measurement payload
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
class IotPayload extends inet::FieldsChunk
{
    string tag; // Identifies the physical property being measured
    double value; // Value of the measurement
    string unit; // If applicable, the unit of the measurement
}