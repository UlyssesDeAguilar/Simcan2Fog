import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.messages.SIMCAN_Message;

//
// Wrapper of messages to be sent as a INET payload.
// The main use case for this class is to wrap SIMCAN messages.
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
class INET_AppMessage extends inet::FieldsChunk
{
    chunkLength = inet::B(20);
	cMessage *appMessage @owned;
}
