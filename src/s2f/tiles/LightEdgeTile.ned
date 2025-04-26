package s2f.tiles;

// Wired comms
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EthernetSwitch;
import s2f.architecture.net.stack.NetworkStack;

// Architectural components from SimCan2Fog models
import s2f.architecture.nodes.iot.controllers.EdgeNode;
import s2f.architecture.nodes.iot.sensors.LuminositySensor;
import s2f.users.edge.EdgeUser;

import inet.node.inet.Router;
import inet.node.inet.WirelessHost;

//
//	Represents an "Edge Scenario" as an reusable "mini bubble" in the
//	grand scheme of the simulation.
//
module LightEdgeTile like ITile
{
    parameters:
        @display("i=misc/town;bgb=20,20;bgi=showcases/floorplan,s;bgu=m");
        string serviceName = default("service");
        int sensors;
        int actuators;
    gates:
        inout ethernet;

    submodules:
        ap: EthernetSwitch {
            @display("i=device/accesspoint;p=6.485207,12.023668");
            @display("p=6.3905325,11.786983");
        }

        user: EdgeUser {
            @display("p=2.745562,5.0177517");
        }

        controller: EdgeNode {
            @display("p=6.485207,5.0177517");
        }

        router: Router {
            @display("p=1.8206645,13.254438");
        }

        sensor[sensors]: LuminositySensor {
            numWlanInterfaces = 0;
            @display("p=12.744652,12.853891");
        }

        actuator[actuators]: WirelessHost {
            numWlanInterfaces = 0;
            @display("i=status/bulb;p=10.8,7.6");
        }

        netStack: NetworkStack {
            @display("p=2.745562,10.508876");
        }

    connections:

        user.serialIn <-- controller.serialOut;
        user.serialOut --> controller.serialIn;

        user.queueIn <-- netStack.queueOut;
        user.queueOut --> netStack.queueIn;

        for i=0..sensors-1 {
            sensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        netStack.ethg++ <--> Eth100M <--> ap.ethg++;
        controller.ethg++ <--> Eth100M <--> ap.ethg++;
        router.ethg++ <--> Eth100M <--> ap.ethg++;
        router.ethg++ <--> ethernet;
}
