package s2f.tiles;

// Wired comms
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.Router;

// Architectural components from SimCan2Fog models
import s2f.users.edge.EdgeUser;
import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.nodes.iot.controllers.Raspberry5;
import s2f.architecture.nodes.iot.sensors.*;
import s2f.architecture.nodes.iot.actuators.*;

//
//	Represents an "Edge Scenario" as an reusable "mini bubble" in the
//	grand scheme of the simulation.
//
module SmartBuilding like ITile
{
    parameters:
        @display("i=misc/building;bgb=29.76,24.16;bgu=m");
        int sensors = 4;
        int actuators = 1;

        // Preloading the user configuration
        user.vms = default([{"name":"my-vm", "type":"VM_large"}]);
        user.remoteApps = default([{"name":"my-service", "type": "MotionDetection","platform":"my-vm"}]);
        user.localApps = default([{"name":"local", "type": "BuildingApp", "dependsOn":"my-service"}]);
        user.sensors = default(["sensor*"]);
        sensor*.model.controllerPath = default("^.^.controller");
        sensor*.model.controllerInterface = default("eth0");
        netStack.nodeTopic = fullName();
        bool enableRouterAsUplink = default(true);
    gates:
        inout ethernet;

    submodules:
        ap: EthernetSwitch {
            @display("i=device/accesspoint;p=9.763314,13.14793");
            @display("p=6.3905325,11.786983");
        }

        user: EdgeUser {
            @display("p=4.621302,3.2544382");
        }

        controller: Raspberry5 {
            @display("p=4.621302,8.656805");
        }

        router: Router {
            @display("p=3.579882,13.213018");
        }

        sensor[sensors]: IpCamera {
            numWlanInterfaces = 0;
            @display("p=18.92,16.72");
        }

        actuator[actuators]: AutomaticWatering {
            numWlanInterfaces = 0;
            @display("p=18.92,8.690001");
        }

        netStack: NetworkStack {
            @display("p=9.763314,5.857989");
        }

    connections:
        user.serialOut --> controller.serialIn;
        user.serialIn <-- controller.serialOut;
        
		user.queueOut --> netStack.queueIn;
		user.queueIn <-- netStack.queueOut;
		
        controller.ethg++ <--> Eth1G <--> ap.ethg++;
        netStack.ethg++ <--> Eth1G <--> ap.ethg++;

        for i=0..sensors-1 {
            sensor[i].ethg++ <--> Eth1G <--> ap.ethg++;
        }

        for i=0..actuators-1 {
            actuator[i].ethg++ <--> Eth1G <--> ap.ethg++;
        }
        
        router.ethg++ <--> Eth1G <--> ap.ethg++ if enableRouterAsUplink;
        router.ethg++ <--> ethernet if enableRouterAsUplink;
        
        ap.ethg++ <--> ethernet if !enableRouterAsUplink;
}
