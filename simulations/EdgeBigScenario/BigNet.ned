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
import s2f.architecture.dns.DnsSimplified;
import s2f.architecture.nodes.cloud.DataCentre;
import s2f.architecture.nodes.fog.FogNode;
import s2f.tiles.ITile;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
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

        @display("bgb=1263.8881,943.896;bgg=50;bgu=km");

    submodules:
        swA: EthernetSwitch {
            @display("p=175.272,747.72003");
        }
        swB: EthernetSwitch {
            @display("p=159.192,305.52");
        }
        swC: EthernetSwitch {
            @display("p=561.192,104.52");
        }
        swD: EthernetSwitch {
            @display("p=976.056,279.792");
        }
        swE: EthernetSwitch {
            @display("p=927.816,734.856");
        }
        swBackbone: EthernetSwitch {

            @display("p=551.544,501.696");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=1178.6641,49.848");
        }

        simData: SimData {
            @display("p=1050.024,49.848");
        }

        dns: DnsSimplified {
            @display("p=611.04,749.328");
        }

        dc: DataCentre {
            @display("p=506.52,749.328");
        }

        fgA1: FogNode {
            @display("p=368.232,685.008");
        }
        fgA2: FogNode {
            @display("p=255.672,612.648");
        }
        fgA3: FogNode {
            @display("p=348.936,601.392");
        }

        fgB1: FogNode {
            @display("p=302.30402,331.24802");
        }
        fgB2: FogNode {
            @display("p=266.928,435.768");
        }
        fgB3: FogNode {

            @display("p=361.8,408.432");
        }
        fgC1: FogNode {
            @display("p=514.56,249.24");
        }
        fgC2: FogNode {
            @display("p=622.296,249.24");
        }
        fgC3: FogNode {

            @display("p=561.192,326.424");
        }
        fgD1: FogNode {
            @display("p=765.408,300.696");
        }
        fgD2: FogNode {
            @display("p=818.472,376.272");
        }
        fgD3: FogNode {
            @display("p=725.208,381.096");
        }
        fgE1: FogNode {
            @display("p=750.936,689.83203");
        }
        fgE2: FogNode {
            @display("p=836.16,578.88");
        }
        fgE3: FogNode {
            @display("p=731.64,599.784");
        }
        usersA[numA]: <> like ITile {
            @display("p=80.4,779.88");
        }
        usersB[numB]: <> like ITile {
            @display("p=80.4,249.24");
        }
        usersC[numC]: <> like ITile {
            @display("p=561.192,32.16");
        }
        usersD[numD]: <> like ITile {
            @display("p=1077.36,242.808");
        }
        usersE[numE]: <> like ITile {
            @display("p=1014.648,773.448");
        }

        localA: InternetCloud {
            @display("p=249.24,723.6");
        }

        localB: InternetCloud {
            @display("p=221.904,353.76");
        }

        localC: InternetCloud {
            @display("p=561.192,176.88");
        }

        localD: InternetCloud {
            @display("p=881.184,307.128");
        }

        localE: InternetCloud {
            @display("p=847.416,694.656");
        }

        metroA: InternetCloud {
            @display("p=413.256,532.248");
        }

        metroB: InternetCloud {
            @display("p=455.064,418.08");
        }
        metroC: InternetCloud {
            @display("p=553.152,408.432");
        }
        metroD: InternetCloud {
            @display("p=643.2,406.824");
        }
        metroE: InternetCloud {

            @display("p=667.32,516.168");
        }
        backbone: InternetCloud {
            @display("p=561.192,599.784");
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
        localA.ethg++ <--> Eth10G <--> fgA1.comm++;
        localA.ethg++ <--> Eth10G <--> fgA2.comm++;
        fgA1.comm++ <--> Eth10G <--> fgA3.comm++;
        fgA2.comm++ <--> Eth10G <--> fgA3.comm++;

        localB.ethg++ <--> Eth10G <--> fgB1.comm++;
        localB.ethg++ <--> Eth10G <--> fgB2.comm++;
        fgB1.comm++ <--> Eth10G <--> fgB3.comm++;
        fgB2.comm++ <--> Eth10G <--> fgB3.comm++;

        localC.ethg++ <--> Eth10G <--> fgC1.comm++;
        localC.ethg++ <--> Eth10G <--> fgC2.comm++;
        fgC1.comm++ <--> Eth10G <--> fgC3.comm++;
        fgC2.comm++ <--> Eth10G <--> fgC3.comm++;

        localD.ethg++ <--> Eth10G <--> fgD1.comm++;
        localD.ethg++ <--> Eth10G <--> fgD2.comm++;
        fgD1.comm++ <--> Eth10G <--> fgD3.comm++;
        fgD2.comm++ <--> Eth10G <--> fgD3.comm++;

        localE.ethg++ <--> Eth10G <--> fgE1.comm++;
        localE.ethg++ <--> Eth10G <--> fgE2.comm++;
        fgE1.comm++ <--> Eth10G <--> fgE3.comm++;
        fgE2.comm++ <--> Eth10G <--> fgE3.comm++;

        //metropolitan.ethg++ <--> Eth10G <--> fg.comm++;
        //metropolitan.ethg++ <--> Eth100G <--> backbone.ethg++;

        // Metropolitan connections
        fgA3.comm++ <--> Eth10G <--> metroA.ethg++;
        fgB3.comm++ <--> Eth10G <--> metroB.ethg++;
        fgC3.comm++ <--> Eth10G <--> metroC.ethg++;
        fgD3.comm++ <--> Eth10G <--> metroD.ethg++;
        fgE3.comm++ <--> Eth10G <--> metroE.ethg++;

        metroA.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroB.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroC.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroD.ethg++ <--> Eth100G <--> swBackbone.ethg++;
        metroE.ethg++ <--> Eth100G <--> swBackbone.ethg++;

        swBackbone.ethg++ <--> Eth100G <--> backbone.ethg++;

        backbone.ethg++ <--> Eth100G <--> dc.comm++;
        backbone.ethg++ <--> Eth100G <--> dns.ethg++;
}
