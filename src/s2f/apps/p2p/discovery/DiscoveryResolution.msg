import inet.networklayer.common.L3Address;
import inet.common.packet.chunk.Chunk;
import inet.common.INETDefs;

//
// Contains a list of IP addresses belonging to possible peer candidates.
// 
message DiscoveryResolution
{
    inet::L3Address resolution[];
};
