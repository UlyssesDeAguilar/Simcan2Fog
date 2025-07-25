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

package s2f.users.generators;
import s2f.architecture.net.stack.NetworkStack;


//
// Wrapper of the Simcan2Cloud user generators in order to
// interact with the Simcan2Fog ecosystem
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
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
