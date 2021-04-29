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

package simcan2.Management.UserGenerators.UserGeneratorCost;
import simcan2.Management.UserGenerators.UserGenerator_simple.UserGenerator_simple;

//
// TODO auto-generated module
//
simple UserGeneratorCost extends UserGenerator_simple
{
    parameters:
        double offerAcceptanceDistribution = default(uniform(0.0,1.0));
        double offerCostIncrease = default(0.5);
        string userTraceSla; 								// Sla of the user from the trace
        
    @class(UserGeneratorCost);
}
