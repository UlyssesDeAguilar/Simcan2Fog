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

package simcan2.simulations.Templates;

// Central component imports
import simcan2.Core.SimData.SimData;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

network BaseNetwork
{
    parameters:
        int xtop = default(200);
        int ytop = default(30);

    submodules:
        simData: SimData {
            @display("p=$xtop,$ytop");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=$xtop,$ytop");
        }
}
