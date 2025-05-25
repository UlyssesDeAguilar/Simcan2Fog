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
// This module processes all outgoing traffic of the MessageQueue
// 
// Key responsibilities:
// - Keep the relationship between queues/topics
// - Create new topics
// - Deliver the messages to the topic consumers
//
// As the consumers register into this service so do their respective topics.
// If it's imperative to allow the creation of queues for topics which are not
// yet registered, because there are no consumers yet,then there's the preemptive
// queue allocation mode. In this mode a queue will be allocated instead of launching
// a model error.
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
simple MessageQueueOut
{
    parameters:
        @class(MessageQueueOut);
        string localAddress = default(""); // The address of the interface where the service will listen (leave empty for all interfaces)
        int localPort; // Port where the service will listen
        bool preemptiveQueueAllocation = default(true); // Whether the service should preallocate a queue with no consumers
    gates:
        input socketIn; // Socket input
        output socketOut; // Socket output
}