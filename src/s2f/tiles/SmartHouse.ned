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
module SmartHouse like ITile
{
    parameters:
        @display("i=misc/town;bgb=20,20;bgi=showcases/floorplan,s;bgu=m");
        int sensors = 2;
        int actuators = 2;
        bool enableRouterAsUplink = default(true);

		// Preloading the user configuration
        user.vms = default([{"name":"my-vm", "type":"VM_small"}]);
        user.remoteApps = default([{"name":"my-service", "type": "SmallIotService","platform":"my-vm"}]);
        user.localApps = default([{"name":"local", "type": "SmallIotApp", "dependsOn":"my-service"}]);
        user.sensors = default(["sensor*"]);    
        sensor*.model.controllerPath = default("^.^.controller");
        sensor*.model.controllerInterface = default("eth0");
        netStack.nodeTopic = fullName();
    gates:
        inout ethernet;

    submodules:
        ap: EthernetSwitch {
            @display("i=device/accesspoint;p=8.702777,11.1424675");
            @display("p=6.3905325,11.786983");
        }

        user: EdgeUser {
            @display("p=4.0782886,7.2098317");
        }

        controller: Raspberry5 {
            @display("p=4.0782886,10.5598545");
        }

        router: Router if enableRouterAsUplink {
            @display("p=1.8206645,13.254438");
        }

        sensor[sensors]: LuminositySensor {
            numWlanInterfaces = 0;
            @display("p=11.797907,14.38325");
        }

        actuator[actuators]: SmartBulb {
            numWlanInterfaces = 0;
            @display("p=11.797907,7.5739646");
        }

        netStack: NetworkStack {
            @display("p=7.282658,7.2098317");
        }

    connections:

        for i=0..sensors-1 {
            sensor[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth100M <--> ap.ethg++;
        }

        user.serialIn <-- controller.serialOut;
        user.serialOut --> controller.serialIn;

        user.queueIn <-- netStack.queueOut;
        user.queueOut --> netStack.queueIn;

        controller.ethg++ <--> Eth10M <--> ap.ethg++;
        netStack.ethg++ <--> Eth10M <--> ap.ethg++;
        
		router.ethg++ <--> Eth10M <--> ap.ethg++ if enableRouterAsUplink;
        router.ethg++ <--> ethernet if enableRouterAsUplink;
        
        ap.ethg++ <--> ethernet if !enableRouterAsUplink;
}
