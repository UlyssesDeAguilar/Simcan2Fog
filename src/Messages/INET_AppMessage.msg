cplusplus {{
	#include "inet/common/packet/chunk/FieldsChunk.h"
	#include "SIMCAN_Message.h"
	using namespace inet;
	typedef SIMCAN_Message *MessagePtr;
}}

class noncobject MessagePtr;
class FieldsChunk;

class INET_AppMessage extends FieldsChunk{
    @customize(true);       // Enable customization of the class
	MessagePtr appMessage;
}
