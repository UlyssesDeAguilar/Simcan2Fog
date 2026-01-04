package s2f.architecture.nodes.iot.actuators;
import inet.node.inet.WirelessHost;

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
