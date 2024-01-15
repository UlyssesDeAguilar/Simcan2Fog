package simcan2.Architecture.Nodes.Iot;
import simcan2.Core.cSIMCAN_Core;

// Simple node that represents an IoT sensor
simple IotSensor extends cSIMCAN_Core{
    parameters:
        double updatePeriod @unit(s) = default(10s);
        @class(IotSensor);
        @display("i=misc/sensor2");
    gates:
        input in;
        output out;
}