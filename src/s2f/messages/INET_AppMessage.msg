import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.messages.SIMCAN_Message;

cplusplus {{
	using namespace inet;
	typedef SIMCAN_Message AppMessage;
}}

class FieldsChunk { @existingClass;@descriptor(false); }
class AppMessage {@existingClass;@descriptor(false);}

class INET_AppMessage extends FieldsChunk{
    @customize(true);       // Enable customization of the class
	AppMessage *appMessage;
}
