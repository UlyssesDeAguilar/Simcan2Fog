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

package s2f.architecture.messagequeue;

//
// This module processes all incoming traffic onto the MessageQueue
// 
// Key responsibilities:
// - Accepting incoming sessions from queue clients 
// - Push the data into the corresponding topic (managed by MessageQueueOut module)
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
simple MessageQueueIn
{
    parameters:
        @class(MessageQueueIn);
        string localAddress = default(""); // The address of the interface where the service will listen (leave empty for all interfaces)
        int localPort; // Port where the service will listen
        string messageQueueOutPath = default("^.messageQueueOut"); // Path to the MessageQueueOut module
    gates:
        input socketIn; // Socket input
        output socketOut; // Socket output
}
