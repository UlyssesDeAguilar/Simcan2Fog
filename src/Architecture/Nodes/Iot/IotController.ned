package simcan2.Architecture.Nodes.Iot;
import simcan2.Core.cSIMCAN_Core;

// Simple node that represents an IoT controller
simple IotController extends cSIMCAN_Core {
    parameters:
        @class(IotController);
    gates:
        input fromFog;
        output toFog;

        // For the time being it's an 1:1 relation but in IoT environement's
        // the Sensors really don't usually recieve communications, maybe just an command
        input fromIot[];
        output toIot[];    
}