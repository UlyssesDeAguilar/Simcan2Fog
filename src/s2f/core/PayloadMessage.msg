import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

cplusplus{{
    #include "inet/common/Ptr.h"
    using PTR = inet::Ptr<const inet::Chunk>;
}}

class PTR {@existingClass; @descriptor(false);}

message PayloadMessage
{
    PTR payload;
}