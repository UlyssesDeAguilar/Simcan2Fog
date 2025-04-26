package s2f.tiles;

// Architectural components from SimCan2Fog models
import s2f.architecture.nodes.iot.controllers.EdgeNode;
import s2f.architecture.nodes.iot.sensors.LuminositySensor;
import s2f.architecture.net.stack.NetworkStack;
import s2f.users.edge.EdgeUser;
import inet.physicallayer.wireless.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;

// Wired comms
import inet.node.ethernet.Eth10M;
import inet.node.wireless.AccessPoint;
import inet.node.inet.Router;
import inet.node.inet.WirelessHost;

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
        **.radioMediumModule = fullPath() + ".radioMedium";
    gates:
        inout ethernet;

    submodules:
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            @display("p=17.372782,2.4615386");
        }

        user: EdgeUser {
            @display("p=7.5739646,5.9171596");
        }

        controller: EdgeNode {
            @display("p=8.804733,11.597633");
        }

        router: Router {
            @display("p=2.0828402,13.254437");
        }

        sensor[sensors]: LuminositySensor {
            @display("p=12.970414,14.011834");
        }

        actuator[actuators]: WirelessHost {
            @display("i=status/bulb;p=13.491124,8.094674");
        }

        netStack: NetworkStack {
            @display("p=5.1597633,10.272189");
        }

    connections:
        user.serialIn <-- controller.serialOut;
        user.serialOut --> controller.serialIn;

        user.queueIn <-- netStack.queueOut;
        user.queueOut --> netStack.queueIn;

        controller.ethg++ <--> Eth10M <--> router.ethg++;
        netStack.ethg++ <--> Eth10M <--> router.ethg++;

        router.ethg++ <--> ethernet;
}
