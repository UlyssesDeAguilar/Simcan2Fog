package s2f.architecture.nodes.iot.actuators;

//
// Represents a smart bulb actuator
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
module SmartBulb extends IotActuator{
    parameters:
        @display("i=light64");
}