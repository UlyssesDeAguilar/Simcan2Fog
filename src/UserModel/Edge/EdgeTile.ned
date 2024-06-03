package simcan2.UserModel.Edge;

import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;


// Wired comms
import inet.node.ethernet.Eth10M;
import inet.node.wireless.AccessPoint;

// Architectural components from SimCan2Fog models
import simcan2.Architecture.Nodes.Iot.EdgeNode;
import simcan2.Architecture.Nodes.Iot.LuminositySensor;
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
    gates:
        inout ethernet;

    submodules:
        user: EdgeUserModel {
            **.radioMediumModule = "^.^.^.radioMedium";
            @display("p=4.6973147,6.008193");
        }
        controller: EdgeNode {
            ns.**.radioMediumModule = "^.^.^.^.radioMedium";
            @display("p=4.6973147,16.131088");
        }
        ap: AccessPoint {
			**.radioMediumModule = "^.^.^.radioMedium";
            @display("p=5.86254,10.778335");
        }
        router: Router {
            @display("p=1.8206645,13.254438");
        }
        
		radioMedium: Ieee80211ScalarRadioMedium;
        
        sensor[sensors]: LuminositySensor {
            **.radioMediumModule = "^.^.^.radioMedium";
            @display("p=12.744652,12.853891");
        }

        actuator[actuators]: WirelessHost {
            **.radioMediumModule = "^.^.^.radioMedium";
            @display("i=status/bulb;p=10.195722,8.73919");
        }

    connections:
        //controller.comm <--> Eth10M <--> router.ethg++;
        //user.ethg++ <--> Eth10M <--> router.ethg++;

        router.ethg++ <--> Eth10M <--> ap.ethg++;
        router.ethg++ <--> ethernet;
}
