package s2f.UserModel.Edge;

// Wired comms
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.EtherSwitch;

// Architectural components from SimCan2Fog models
import s2f.Architecture.Nodes.Iot.*;
import inet.node.inet.Router;

//
//	Represents an "Edge Scenario" as an reusable "mini bubble" in the
//	grand scheme of the simulation.
//
module SmartBuilding like ITile
{
    parameters:
        @display("i=misc/building;bgb=29.76,24.16;bgu=m");
        string serviceName;
        int sensors = 4;
        int actuators = 1;
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
            hy.apps = "BuildingApp";
            @display("p=7.04,15.76");
        }

        router: Router {
            @display("p=1.8206645,13.254438");
        }

        sensor[sensors]: IpCamera {
            numWlanInterfaces = 0;
            @display("p=18.16,11.52");
        }

        actuator[actuators]: AutomaticWatering {
            numWlanInterfaces = 0;
            @display("p=17.6,5.04");
        }

    connections:
        controller.comm <--> Eth1G <--> ap.ethg++;
        user.ethg++ <--> Eth1G <--> ap.ethg++;

        for i=0..sensors-1 {
            sensor[i].ethg++ <--> Eth1G <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth1G <--> ap.ethg++;
        }

        router.ethg++ <--> Eth1G <--> ap.ethg++;
        router.ethg++ <--> ethernet;
}
