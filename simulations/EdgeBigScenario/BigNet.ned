//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package s2f.simulations.EdgeBigScenario;

import s2f.core.simdata.SimData;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import s2f.architecture.dns.DnsSimplified;
import s2f.architecture.messagequeue.MessageQueue;
import s2f.tiles.ITile;

import s2f.management.cloudprovider.CloudProvider;
import s2f.architecture.nodes.cloud.DataCentre;
import s2f.architecture.nodes.fog.FogNode;

import inet.node.internetcloud.InternetCloud;
import inet.node.inet.Router;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth100G;
import inet.node.ethernet.EthernetSwitch;

network BigNet
{
    parameters:
        int numA = 20;
        int numB = 20;
        int numC = 20;
        int numD = 20;
        int numE = 20;

        @display("bgb=1579.596,1161.022;bgg=50;bgu=km");

    submodules:
        swA: EthernetSwitch {
            @display("p=172.35399,946.99994");
        }
        swB: EthernetSwitch {
            @display("p=172.35399,335.23798");
        }
        swC: EthernetSwitch {
            @display("p=695.09796,109.852");
        }
        swD: EthernetSwitch {
            @display("p=1272.768,308.722");
        }
        swE: EthernetSwitch {
            @display("p=1284.132,857.98193");
        }

        swLocalA: EthernetSwitch {
            @display("p=320.086,831.46594");
        }
        swLocalB: EthernetSwitch {
            @display("p=325.76797,446.98398");
        }
        swLocalC: EthernetSwitch {
            @display("p=695.09796,270.84198");
        }
        swLocalD: EthernetSwitch {
            @display("p=1092.8379,375.012");
        }
        swLocalE: EthernetSwitch {
            @display("p=1109.8839,772.75195");
        }
        swMetroA: EthernetSwitch {
            @display("p=473.49997,691.31");
        }
        swMetroB: EthernetSwitch {
            @display("p=507.59198,522.74396");
        }
        swMetroC: EthernetSwitch {
            @display("p=685.628,446.98398");
        }
        swMetroD: EthernetSwitch {
            @display("p=899.64996,454.55997");
        }
        swMetroE: EthernetSwitch {
            @display("p=916.696,691.31");
        }
        swBackbone: EthernetSwitch {
            @display("p=685.628,607.974");
        }
        swGlobal: EthernetSwitch;
        configurator: Ipv4NetworkConfigurator {
            @display("p=1479.214,49.244");
        }

        simData: SimData {
            @display("p=1342.846,49.244");
        }

        dns: DnsSimplified {
            @display("p=850.40594,893.96796");
        }

        cloudProvider: CloudProvider {
            @display("p=651.53595,948.894");
        }
        messageQueue: MessageQueue {

            @display("p=776.54,948.894");
        }
        dc: DataCentre {
            @display("p=560.62396,893.96796");
        }

        fgA1: FogNode {
            @display("p=454.55997,816.31396");
        }
        fgA2: FogNode {
            @display("p=325.76797,719.72");
        }
        fgA3: FogNode {
            @display("p=397.74,761.38794");
        }

        fgB1: FogNode {
            @display("p=435.61996,424.25598");
        }
        fgB2: FogNode {
            @display("p=359.86,558.73");
        }
        fgB3: FogNode {

            @display("p=416.68,500.016");
        }
        fgC1: FogNode {
            @display("p=589.034,337.132");
        }
        fgC2: FogNode {
            @display("p=801.162,337.132");
        }
        fgC3: FogNode {

            @display("p=685.628,352.284");
        }
        fgD1: FogNode {
            @display("p=973.5159,352.284");
        }
        fgD2: FogNode {
            @display("p=1064.428,494.33398");
        }
        fgD3: FogNode {
            @display("p=1005.7139,429.938");
        }
        fgE1: FogNode {
            @display("p=977.30396,804.94995");
        }
        fgE2: FogNode {
            @display("p=1109.8839,664.79395");
        }
        fgE3: FogNode {
            @display("p=1022.75995,723.508");
        }
        usersA[numA]: <> like ITile {
            @display("p=79.548,1009.50195");
        }
        usersB[numB]: <> like ITile {
            @display("p=80.4,249.24");
        }
        usersC[numC]: <> like ITile {
            @display("p=685.628,35.986");
        }
        usersD[numD]: <> like ITile {
            @display("p=1373.1499,272.736");
        }
        usersE[numE]: <> like ITile {
            @display("p=1373.1499,893.96796");
        }

        localA: InternetCloud {
            @display("p=246.21999,893.96796");
        }

        localB: InternetCloud {
            @display("p=246.21999,393.952");
        }

        localC: InternetCloud {
            @display("p=695.09796,187.506");
        }

        localD: InternetCloud {
            @display("p=1178.068,337.132");
        }

        localE: InternetCloud {
            @display("p=1210.266,804.94995");
        }

        metroA: InternetCloud {
            @display("p=560.62396,647.748");
        }

        metroB: InternetCloud {
            @display("p=589.034,553.048");
        }
        metroC: InternetCloud {
            @display("p=685.628,524.638");
        }
        metroD: InternetCloud {
            @display("p=786.00995,524.638");
        }
        metroE: InternetCloud {

            @display("p=801.162,609.868");
        }
        backbone: InternetCloud {
            @display("p=695.09796,693.204");
        }
    connections:

        // Users - Regional
        for i=0..sizeof(usersA)-1 {
            usersA[i].ethernet <--> Eth10G <--> swA.ethg++;
        }

        for i=0..sizeof(usersB)-1 {
            usersB[i].ethernet <--> Eth10G <--> swB.ethg++;
        }

        for i=0..sizeof(usersC)-1 {
            usersC[i].ethernet <--> Eth10G <--> swC.ethg++;
        }

        for i=0..sizeof(usersD)-1 {
            usersD[i].ethernet <--> Eth10G <--> swD.ethg++;
        }

        for i=0..sizeof(usersE)-1 {
            usersE[i].ethernet <--> Eth10G <--> swE.ethg++;
        }

        swA.ethg++ <--> Eth10G <--> localA.ethg++;
        swB.ethg++ <--> Eth10G <--> localB.ethg++;
        swC.ethg++ <--> Eth10G <--> localC.ethg++;
        swD.ethg++ <--> Eth10G <--> localD.ethg++;
        swE.ethg++ <--> Eth10G <--> localE.ethg++;

        // Regional - Fog Nodes
        localA.ethg++ <--> Eth10G <--> swLocalA.ethg++;
        fgA1.comm++ <--> Eth10G <--> swLocalA.ethg++;
        fgA2.comm++ <--> Eth10G <--> swLocalA.ethg++;
        fgA3.comm++ <--> Eth10G <--> swLocalA.ethg++;

        localB.ethg++ <--> Eth10G <--> swLocalB.ethg++;
        fgB1.comm++ <--> Eth10G <--> swLocalB.ethg++;
        fgB2.comm++ <--> Eth10G <--> swLocalB.ethg++;
        fgB3.comm++ <--> Eth10G <--> swLocalB.ethg++;

        localC.ethg++ <--> Eth10G <--> swLocalC.ethg++;
        fgC1.comm++ <--> Eth10G <--> swLocalC.ethg++;
        fgC2.comm++ <--> Eth10G <--> swLocalC.ethg++;
        fgC3.comm++ <--> Eth10G <--> swLocalC.ethg++;

        localD.ethg++ <--> Eth10G <--> swLocalD.ethg++;
        fgD1.comm++ <--> Eth10G <--> swLocalD.ethg++;
        fgD2.comm++ <--> Eth10G <--> swLocalD.ethg++;
        fgD3.comm++ <--> Eth10G <--> swLocalD.ethg++;

        localE.ethg++ <--> Eth10G <--> swLocalE.ethg++;
        fgE1.comm++ <--> Eth10G <--> swLocalE.ethg++;
        fgE2.comm++ <--> Eth10G <--> swLocalE.ethg++;
        fgE3.comm++ <--> Eth10G <--> swLocalE.ethg++;

        //metropolitan.ethg++ <--> Eth10G <--> fg.comm++;
        //metropolitan.ethg++ <--> Eth100G <--> backbone.ethg++;

        // Metropolitan connections
        metroA.ethg++ <--> Eth10G <--> swMetroA.ethg++;
        fgA1.comm++ <--> Eth10G <--> swMetroA.ethg++;
        fgA2.comm++ <--> Eth10G <--> swMetroA.ethg++;
        fgA3.comm++ <--> Eth10G <--> swMetroA.ethg++;

        metroB.ethg++ <--> Eth10G <--> swMetroB.ethg++;
        fgB1.comm++ <--> Eth10G <--> swMetroB.ethg++;
        fgB2.comm++ <--> Eth10G <--> swMetroB.ethg++;
        fgB3.comm++ <--> Eth10G <--> swMetroB.ethg++;

        metroC.ethg++ <--> Eth10G <--> swMetroC.ethg++;
        fgC1.comm++ <--> Eth10G <--> swMetroC.ethg++;
        fgC2.comm++ <--> Eth10G <--> swMetroC.ethg++;
        fgC3.comm++ <--> Eth10G <--> swMetroC.ethg++;

        metroD.ethg++ <--> Eth10G <--> swMetroD.ethg++;
        fgD1.comm++ <--> Eth10G <--> swMetroD.ethg++;
        fgD2.comm++ <--> Eth10G <--> swMetroD.ethg++;
        fgD3.comm++ <--> Eth10G <--> swMetroD.ethg++;

        metroE.ethg++ <--> Eth10G <--> swMetroE.ethg++;
        fgE1.comm++ <--> Eth10G <--> swMetroE.ethg++;
        fgE2.comm++ <--> Eth10G <--> swMetroE.ethg++;
        fgE3.comm++ <--> Eth10G <--> swMetroE.ethg++;

		// Backbone connections
        metroA.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroB.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroC.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroD.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroE.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        swBackbone.ethg++ <--> Eth100G <--> backbone.ethg++;

		// Global or Core connections
        swGlobal.ethg++ <--> Eth100G <--> dc.comm++;
        swGlobal.ethg++ <--> Eth100G <--> dns.ethg++;
        swGlobal.ethg++ <--> Eth100G <--> messageQueue.ethg++;
        swGlobal.ethg++ <--> Eth100G <--> cloudProvider.ethg++;
        swGlobal.ethg++ <--> Eth100G <--> backbone.ethg++;
}
