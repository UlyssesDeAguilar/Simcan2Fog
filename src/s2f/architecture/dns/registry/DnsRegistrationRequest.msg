import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import s2f.architecture.net.protocol.RestfulRequest;
import s2f.architecture.dns.ResourceRecord;

namespace dns;

//
// Request for registering/deleting DNS records
//
class DnsRegistrationRequest extends RestfulRequest
{
	chunkLength = inet::B(20);	   // Packet length
	string zone;				   // Zone where we can insert the records
	ResourceRecord records[];      // Domain bindings
}