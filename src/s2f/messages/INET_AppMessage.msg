import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.messages.SIMCAN_Message;

cplusplus {{
	typedef SIMCAN_Message AppMessage;
}}

class AppMessage {@existingClass;@descriptor(false);}

class INET_AppMessage extends inet::FieldsChunk{
    @customize(true);       // Enable customization of the class
	AppMessage *appMessage;
}
