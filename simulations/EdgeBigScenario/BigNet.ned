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

        @display("bgb=1000,800;bgg=50;bgu=km");

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=906.912,51.456");
        }

        simData: SimData {
            @display("p=785.91003,50.25");
        }

        dns: DnsSimplified {
            @display("p=541.896,553.152");
        }

        dc: DataCentre {
            @display("p=437.376,551.544");
        }

        fgA1: FogNode {
            @display("p=311.952,556.368");
        }
        fgA2: FogNode {
            @display("p=199.392,484.008");
        }
        fgA3: FogNode {
            @display("p=292.656,472.752");
        }

        fgB1: FogNode {
            @display("p=246.024,202.608");
        }
        fgB2: FogNode {
            @display("p=210.648,307.128");
        }
        fgB3: FogNode {

            @display("p=305.52,279.792");
        }
        fgC1: FogNode {
            @display("p=458.28,120.6");
        }
        fgC2: FogNode {
            @display("p=566.016,120.6");
        }
        fgC3: FogNode {

            @display("p=504.912,197.784");
        }
        fgD1: FogNode {
            @display("p=709.128,172.056");
        }
        fgD2: FogNode {
            @display("p=762.192,247.632");
        }
        fgD3: FogNode {

            @display("p=668.928,252.456");
        }
        fgE1: FogNode {
            @display("p=696.264,561.192");
        }
        fgE2: FogNode {
            @display("p=779.88,450.24");
        }
        fgE3: FogNode {

            @display("p=675.36,471.144");
        }
        usersA[numA]: <> like ITile {
            @display("p=117.384,685.008");
        }
        usersB[numB]: <> like ITile {
            @display("p=82.008,144.72");
        }
        usersC[numC]: <> like ITile {
            @display("p=368.232,36.984");
        }
        usersD[numD]: <> like ITile {
            @display("p=903.696,236.376");
        }
        usersE[numE]: <> like ITile {
            @display("p=868.32,633.552");
        }

        localA: InternetCloud {
            @display("p=194.568,594.96");
        }

        localB: InternetCloud {
            @display("p=165.62401,225.12");
        }

        localC: InternetCloud {
            @display("p=514.56,38.592");
        }

        localD: InternetCloud {
            @display("p=824.904,178.488");
        }

        localE: InternetCloud {
            @display("p=791.136,566.016");
        }

        metroA: InternetCloud {
            @display("p=356.976,403.608");
        }

        metroB: InternetCloud {
            @display("p=398.784,289.44");
        }
        metroC: InternetCloud {
            @display("p=496.872,279.792");
        }
        metroD: InternetCloud {
            @display("p=586.92,278.184");
        }
        metroE: InternetCloud {

            @display("p=611.04,387.528");
        }
        backbone: InternetCloud {
            @display("p=493.656,402");
        }
    connections:

        // Users - Regional
        for i=0..sizeof(usersA)-1 {
            usersA[i].ethernet <--> Eth10G <--> localA.ethg++;
        }

        for i=0..sizeof(usersB)-1 {
            usersB[i].ethernet <--> Eth10G <--> localB.ethg++;
        }

        for i=0..sizeof(usersC)-1 {
            usersC[i].ethernet <--> Eth10G <--> localC.ethg++;
        }

        for i=0..sizeof(usersD)-1 {
            usersD[i].ethernet <--> Eth10G <--> localD.ethg++;
        }

        for i=0..sizeof(usersE)-1 {
            usersE[i].ethernet <--> Eth10G <--> localE.ethg++;
        }

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

        metroA.ethg++ <--> Eth100G <--> backbone.ethg++;
        metroB.ethg++ <--> Eth100G <--> backbone.ethg++;
        metroC.ethg++ <--> Eth100G <--> backbone.ethg++;
        metroD.ethg++ <--> Eth100G <--> backbone.ethg++;
        metroE.ethg++ <--> Eth100G <--> backbone.ethg++;

        backbone.ethg++ <--> Eth100G <--> dc.comm++;
        backbone.ethg++ <--> Eth100G <--> dns.ethg++;
}
