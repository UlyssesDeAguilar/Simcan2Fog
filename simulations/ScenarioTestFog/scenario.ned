package s2f.simulations.ScenarioTestFog;

import s2f.Architecture.Nodes.Iot.*;
import s2f.Architecture.Nodes.Fog.FogNode;
import s2f.Architecture.Nodes.Cloud.CloudBlade;
import s2f.Architecture.Nodes.Cloud.CloudBoard;
import s2f.Architecture.Nodes.Cloud.CloudRack;
import s2f.Management.DataCentreManagers.DataCentreManagerFirstFit.DataCentreManagerFirstFit;
import s2f.Management.CloudProviders.*.*;
import s2f.Architecture.Network.DataCentreNetwork.*;
import s2f.Management.UserGenerators.*.*;


// ---------------------------------------------------------------
//   Definition of the scenario
// ---------------------------------------------------------------
network ScenarioTest
{
    parameters:
        string appList;
        string vmList;
        string slaList;
        string userList;
        string dataCentresList;
        int numControllers = default(2);
        int numSensors = default(2);

        @display("bgb=844,303");
    types:
        channel RealChannel extends ned.DatarateChannel
        {
            datarate = 200 Mbps;
            delay = 10 ms;
            //@statistic[bandwidthPercentage](source="packetBits(messageSent) / (" + string(datarate) +"1024*1024*8)";record=last);
        }
    // -------------------------------------------------------
    //   Definition of main modules
    // -------------------------------------------------------
    submodules:

        // ---------------------------------------------------------------
        //   Definition of the data-centres
        // ---------------------------------------------------------------
        dc_DataCentre[1]: DataCentre_A {
            parameters:
                appList = appList;
                vmList = vmList;
                slaList = slaList;
                userList = userList;
                @display("p=679,62");
        }

        fog_node: FogNode {
            @display("p=416,114");
        }

        controller[numControllers]: IotController {

            //@display("p=138,114");
        }
        sensor[numSensors*numControllers]: IotSensor {
            //@display("p=138,227");
        }
    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections between the cloud provider and each data-centre manager
        fog_node.toDataCentre --> ned.IdealChannel --> dc_DataCentre[0].fromCloudProvider;
        fog_node.fromDataCentre <-- ned.IdealChannel <-- dc_DataCentre[0].toCloudProvider;

        for i=0..numControllers-1 {
            fog_node.toEdge++ --> RealChannel --> controller[i].fromFog;
            fog_node.fromEdge++ <-- RealChannel <-- controller[i].toFog;
        }

        for i=0..numControllers-1, for j=0..numSensors-1 {
            controller[i].toIot++ --> sensor[j+i*numSensors].in;
            controller[i].fromIot++ <-- sensor[j+i*numSensors].out;
        }
}

network ScenarioTestEdgeFog
{
    parameters:
        @display("bgb=610,278");
    types:
        channel RealChannel extends ned.DatarateChannel
        {
            datarate = 200 Mbps;
            delay = 10 ms;
            //@statistic[bandwidthPercentage](source="packetBits(messageSent) / (200*1024*1024)"; record=last);
        }

    submodules:

        fog_node: FogNode {
            @display("p=380,160");
        }
        controller: IotController {
            @display("p=264,160");
        }
        sensor: IotSensor {
            @display("p=137,160");
        }
    connections allowunconnected:
        fog_node.toEdge++ --> RealChannel --> controller.fromFog;
        fog_node.fromEdge++ <-- RealChannel <-- controller.toFog;

        controller.toIot++ --> sensor.in;
        controller.fromIot++ <-- sensor.out;
}
