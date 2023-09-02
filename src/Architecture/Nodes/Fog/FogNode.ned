package simcan2.Architecture.Nodes.Fog;
import simcan2.Core.cSIMCAN_Core; // As it does inherit from this base class

// Simple node that represents a FogNode
// For the time being it has a simple interface
simple FogNode extends cSIMCAN_Core {
    parameters:
        double storageSize @unit(GB) = default(2000);
        double storageBandWidth @unit(Mbps) = default(10);  // It's Mbit/s
        @class(FogNode);                                    // Important for the NED builder
    gates:
        input fromDataCentre;
        output toDataCentre;

        input fromEdge[];
        output toEdge[];
}