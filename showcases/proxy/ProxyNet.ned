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
package proxy;

import s2f.apps.UserAppModule;
import s2f.architecture.net.stack.proxy.AppProxy;
import s2f.architecture.net.stack.proxy.ServiceTable;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.StandardHost;

module SimplifiedHost extends StandardHost
{
    submodules:
        hostedApp: UserAppModule {
            @display("p=932.554,60.966003");
        }
        proxy: AppProxy {
            @display("p=749.656,60.966003");
        }
        serviceTable: ServiceTable {
            @display("p=1108.678,221.28401");
        }
    connections:
        // App connects to the proxy
        hostedApp.socketIn <-- proxy.appOut++;
        hostedApp.socketOut --> proxy.appIn++;

        // Proxy connects to the TCP/UDP Layer
        proxy.transportOut --> at.in++;
        proxy.transportIn <-- at.out++;
}

//
// Network that showcases the essential function of the proxy module
//
network ProxyNet
{
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=67,36");
        }
        switch: EthernetSwitch {
            @display("p=214,96");
        }
        client: SimplifiedHost {
            @display("p=140,103");
        }
        server: SimplifiedHost {

            @display("p=282,103");
        }
    connections:
        server.ethg++ <--> Eth10M <--> switch.ethg++;
        client.ethg++ <--> Eth10M <--> switch.ethg++;
}
