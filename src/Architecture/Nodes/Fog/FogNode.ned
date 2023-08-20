package simcan2.Architecture.Nodes.Fog;
import simcan2.Core.cSIMCAN_Core; // As it does inherit from this base class

// Simple node that represents a FogNode
// For the time being it has a simple interface
simple FogNode extends cSIMCAN_Core {
    parameters:
        @class(FogNode); // Important for the NED builder
    gates:
        input in;
        output out;
}