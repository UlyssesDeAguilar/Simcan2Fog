import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.messages.SIMCAN_Message;

class INET_AppMessage extends inet::FieldsChunk
{
	cMessage *appMessage @owned;
}
