package s2f.tiles;

// Wired comms
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.Router;

// Architectural components from SimCan2Fog models
import s2f.users.edge.EdgeUser;
import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.nodes.iot.controllers.Raspberry5;
import s2f.architecture.nodes.iot.sensors.*;
import s2f.architecture.nodes.iot.actuators.*;
import inet.node.ethernet.Eth10M;

//
//	Represents an "Edge Scenario" as an reusable "mini bubble" in the
//	grand scheme of the simulation.
//
module SmartFarm like ITile
{
    parameters:
        @display("i=misc/town;bgb=29.76,24.16;bgi=background/green,s;bgu=m");
        int tsensors = 4;
        int hsensors = 4;
        int actuators = 2;

        // Preloading the user configuration
        user.vms = default([{"name":"my-vm", "type":"VM_small"}]);
        user.remoteApps = default([{"name":"my-service", "type": "FarmManager","platform":"my-vm"}]);
        user.localApps = default([{"name":"local", "type": "FarmApp", "dependsOn":"my-service"}]);
        user.sensors = default(["hsensor*", "tsensor*"]);
        *sensor*.model.controllerPath = default("^.^.controller");
        *sensor*.model.controllerInterface = default("eth0");
        netStack.nodeTopic = fullName();
        bool enableRouterAsUplink = default(true);
    gates:
        inout ethernet;

    submodules:
        ap: EthernetSwitch {
            @display("i=device/accesspoint;p=9.177516,12.497043");
            @display("p=6.3905325,11.786983");
        }

        user: EdgeUser {
            @display("p=3.579882,4.100592");
        }

        controller: Raspberry5 {
            //hy.apps = "FarmApp";
            @display("p=5.467456,9.502959");
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

        netStack: NetworkStack {
            @display("p=9.177516,4.100592");
        }

    connections:
        user.serialIn <-- controller.serialOut;
        user.serialOut --> controller.serialIn;

        user.queueIn <-- netStack.queueOut;
        user.queueOut --> netStack.queueIn;

        controller.ethg++ <--> Eth100M <--> ap.ethg++;
        netStack.ethg++ <--> Eth100M <--> ap.ethg++;

        for i=0..tsensors-1 {
            tsensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..hsensors-1 {
            hsensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }
        
        router.ethg++ <--> Eth100M <--> ap.ethg++ if enableRouterAsUplink;
        router.ethg++ <--> ethernet if enableRouterAsUplink;
        
        ap.ethg++ <--> ethernet if !enableRouterAsUplink;
}
