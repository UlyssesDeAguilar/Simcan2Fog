package s2f.architecture.nodes.iot.actuators;
import inet.node.inet.WirelessHost;

//
// Iot Actuator: This is a module that integrates the IotActuatorModel
// into an INET based WirelessHost. This class can be extended for acting
// as a base of other actuator models.
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
module IotActuator extends WirelessHost
{
    submodules:
        model: IotActuatorModel {
            @display("p=750,61");
        }
    connections:
        model.socketIn <-- at.out++;
        model.socketOut --> at.in++;
}
