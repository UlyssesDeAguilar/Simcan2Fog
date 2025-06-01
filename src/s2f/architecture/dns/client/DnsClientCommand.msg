import inet.common.INETDefs;
import inet.common.packet.Packet;
import inet.networklayer.common.L3Address;

namespace s2f::dns;

enum DnsClientResult
{
    OK = 0;
    ERROR = 1;
}

//
// Petition for the DnsClientService in order to try to resolve a DNS query
// 
// The ipPool will usually be the list of DNS resolvers to be tried 
// before giving up. The payload is expected to be a DNS query and
// will be sent as is.
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
message DnsClientCommand
{
    inet::L3Address ipPool[]; // List of server ip addresses where the query (payload) will be sent
    uint16_t requestId; // Request Id set by the module that creates the request
    inet::Packet *payload @owned; // Packet containing the DNS query
}

//
// Result of a DnsClientCommand
// 
// The ipPool will usually be the list of DNS resolvers to be tried 
// before giving up. The payload is expected to be a DNS query and
// will be sent as is.
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
message DnsClientIndication
{
    uint16_t requestId; // Request Id set by the module that created the original command
    DnsClientResult result; // The result of the operation
    inet::Packet *payload @owned; // The response or nullpointer if there was an error
}