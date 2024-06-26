package s2f.UserModel.Edge;

// Wired comms
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EtherSwitch;

// Architectural components from SimCan2Fog models
import s2f.Architecture.Nodes.Iot.*;
import inet.node.inet.Router;

//
//	Represents an "Edge Scenario" as an reusable "mini bubble" in the
//	grand scheme of the simulation.
//
module SmartHouse like ITile
{
    parameters:
        @display("i=misc/town;bgb=20,20;bgi=showcases/floorplan,s;bgu=m");
        string serviceName;
        int sensors = 2;
        int actuators = 2;
    gates:
        inout ethernet;

    submodules:
        ap: EtherSwitch {
            @display("i=device/accesspoint;p=6.485207,11.029586");
            @display("p=6.3905325,11.786983");
        }
        
        user: EdgeUserModel {
            @display("p=4.6973147,6.008193");
        }

        controller: Raspberry5 {
            hy.apps = "SmartHomeAppEndToEnd";
            @display("p=4.733728,15.668639");
        }

        router: Router {
            @display("p=1.8206645,13.254438");
        }

        sensor[sensors]: LuminositySensor {
            numWlanInterfaces = 0;
            @display("p=12.744652,12.853891");
        }

        actuator[actuators]: SmartBulb {
            numWlanInterfaces = 0;
            @display("p=10.8,7.6");
        }

    connections:
        controller.comm <--> Eth100M <--> ap.ethg++;
        user.ethg++ <--> Eth100M <--> ap.ethg++;

        for i=0..sensors-1 {
            sensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }
		
        router.ethg++ <--> Eth100M <--> ap.ethg++;
        router.ethg++ <--> ethernet;
}
