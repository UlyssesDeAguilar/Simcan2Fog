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
module SmartFactory like ITile
{
    parameters:
        @display("i=misc/town2,,0;bgb=29.76,24.16,,,0;bgu=m");
        int tsensors = 10;
        int actuators = 2;

         // Preloading the user configuration
        user.vms = default([{"name":"my-vm", "type":"VM_medium"}]);
        user.remoteApps = default([{"name":"my-service", "type": "FactoryService","platform":"my-vm"}]);
        user.localApps = default([{"name":"local", "type": "FactoryApp", "dependsOn":"my-service"}]);
        user.sensors = default(["tsensor*"]);
        *sensor*.model.controllerPath = default("^.^.controller");
        *sensor*.model.controllerInterface = default("eth0");
        netStack.nodeTopic = fullName();
    gates:
        inout ethernet;

    submodules:
        ap: EthernetSwitch {
            @display("i=device/accesspoint;p=10.915385,14.469232");
            @display("p=6.3905325,11.786983");
        }

        user: EdgeUser {
            @display("p=4.0615387,3.8076925");
        }

        controller: Raspberry5 {
            //hy.apps = "FactoryApp";
            @display("p=7.361539,9.138462");
        }

        router: Router {
            @display("p=3.046154,13.115385");
        }

        tsensor[tsensors]: HeatSensor {
            numWlanInterfaces = 0;
            @display("p=20.053848,15.907693");
        }

        actuator[actuators]: AutomaticWatering {
            numWlanInterfaces = 0;
            @display("p=18.7,7.9538465");
        }

        netStack: NetworkStack {
            @display("p=11.676924,3.8076925");
        }

    connections:
        user.serialOut --> controller.serialIn;
        user.serialIn <-- controller.serialOut;

        user.queueOut --> netStack.queueIn;
        user.queueIn <-- netStack.queueOut;

        controller.ethg++ <--> Eth100M <--> ap.ethg++;
        netStack.ethg++ <--> Eth100M <--> ap.ethg++;

        for i=0..tsensors-1 {
            tsensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        router.ethg++ <--> Eth100M <--> ap.ethg++;
        router.ethg++ <--> ethernet;
}
