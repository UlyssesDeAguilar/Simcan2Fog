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

package simcan2.Management.UserGenerators;
import simcan2.Architecture.Network.Stack.NetworkStack;

module UserGenerator
{
    @display("bgb=197,144;i=abstract/people");
    gates:
        inout comm;
    submodules:
        ns: NetworkStack {
            nodeTopic = "UserGenerator";
            @display("p=49,62");
        }
        generator: <default("UserGenerator_simple")> like IUserGenerator {
            @display("p=138,61");
        }
    connections allowunconnected:
        ns.queueIn <-- generator.toCloudProvider;
        ns.queueOut --> generator.fromCloudProvider;
        ns.ethg++ <--> comm;
}
