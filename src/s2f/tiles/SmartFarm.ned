package s2f.tiles;

// Wired comms
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;

// Architectural components from SimCan2Fog models
import s2f.architecture.nodes.iot.controllers.Raspberry5;
import s2f.architecture.nodes.iot.sensors.*;
import s2f.architecture.nodes.iot.actuators.*;

//
//	Represents an "Edge Scenario" as an reusable "mini bubble" in the
//	grand scheme of the simulation.
//
module SmartFarm like ITile
{
    parameters:
        @display("i=misc/town;bgb=29.76,24.16;bgi=background/green,s;bgu=m");
        string serviceName;
        int tsensors = 4;
        int hsensors = 4;
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
            hy.apps = "FarmApp";
            @display("p=7.04,15.76");
        }

        router: Router {
            @display("p=1.8206645,13.254438");
        }

        tsensor[tsensors]: HeatSensor {
            numWlanInterfaces = 0;
            @display("p=18.16,11.52");
        }

        hsensor[hsensors]: HumiditySensor {
            numWlanInterfaces = 0;
            @display("p=14.72,17.2");
        }

        actuator[actuators]: AutomaticWatering {
            numWlanInterfaces = 0;
            @display("p=17.6,5.04");
        }

    connections:
        controller.comm <--> Eth100M <--> ap.ethg++;
        user.ethg++ <--> Eth100M <--> ap.ethg++;

        for i=0..tsensors-1 {
            tsensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..hsensors-1 {
            hsensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        router.ethg++ <--> Eth100M <--> ap.ethg++;
        router.ethg++ <--> ethernet;
}
