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

package s2f.architecture.net.hub;
import s2f.core.cSIMCAN_Core;
//
// TODO auto-generated module
//
simple CheckingHubModule extends cSIMCAN_Core{
    
    parameters:
        bool staticAppAssignment;			// True if the assignment of application is static or False in another case
    	@class(CheckingHubModule);
    	
    gates:
        input fromInput[];					// Gates from the inputs
        input fromOutput[];					// Gates from the outputs
        
        output toInput[];					// Gates to the inputs
        output toOutput[];					// Gates to the outputs
}
