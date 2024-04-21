package simcan2.UserModel.Edge;

// This includes the simulation of the medium of propagation for the standard IEEE 802.15.4
import inet.physicallayer.apskradio.packetlevel.ApskScalarRadioMedium;
import inet.physicallayer.ieee802154.bitlevel.Ieee802154UwbIrRadioMedium;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;

// Architectural components from SimCan2Fog models
import simcan2.Architecture.Nodes.Iot.EdgeNode;
import inet.node.inet.Router;
import inet.node.inet.SensorNode;

//
//	Represents an "Edge Scenario" as an reusable "mini bubble" in the
//	grand scheme of the simulation.
//
module EdgeTile
{
    parameters:
        @display("i=HouseIcon;bgb=20,20;bgi=showcases/floorplan,s");
        int sensors;
        int actuators;

    gates:
        inout ethernet;

    submodules:
        user: EdgeUserModel {
            @display("p=4.6973147,6.008193");
        }
        device: EdgeNode {

            @display("p=4.6973147,16.131088");
        }
        router: Router {
            @display("p=4.6973147,11.579427");
        }

        sensor[sensors]: SensorNode {
            @display("i=misc/sensor;p=12.125626,15.075103");
        }

        actuator[actuators]: SensorNode {
            @display("i=status/bulb;p=10.195722,8.73919");
        }

    connections:
        device.comm <--> router.ethg++;
        user.ethg++ <--> router.ethg++;
        
        router.ethg++ <--> ethernet;
}
