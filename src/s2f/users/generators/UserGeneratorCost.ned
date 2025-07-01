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

//
// User generator that takes priorities and costs via SLAs into account
//
// @author Pablo Cerro Cañizares
// @author Adrian Bernal
// @author Ulysses de Aguilar
// @version 1.3
simple UserGeneratorCost extends UserGenerator_simple
{
    parameters:
        @class(UserGeneratorCost);
        double offerAcceptanceDistribution = default(uniform(0.0,1.0)); // Acceptance distribution of the user instance
        double offerCostIncrease = default(0.5); // Increase in the offer's cost 
        string userTraceSla; // SLAs of the users in a string format. FIXME: Remove this
}
