import inet.common.INETDefs;
import inet.common.packet.Packet;
import inet.networklayer.common.L3Address;

namespace dns;

enum DnsClientResult
{
    OK = 0;
    ERROR = 1;
}

//
//  Should be set on the packet control info!
//
message DnsClientCommand
{
    inet::L3Address ipPool[];
    uint16_t requestId;
    inet::Packet *payload @owned;
}

//
// This is the result, if OK then the message itself is an Packet, cMessage otherwise.
//
message DnsClientIndication
{
    uint16_t requestId;
    DnsClientResult result;
    inet::Packet *payload @owned;
}