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

import s2f.architecture.messagequeue.internals.SMQueue;
import s2f.architecture.messagequeue.services.PushServer;
import s2f.architecture.messagequeue.services.PullServer;

import inet.applications.tcpapp.TcpServerApp;
import inet.queueing.buffer.PacketBuffer;
import inet.node.inet.StandardHost;


module MessageQueue extends StandardHost
{
    parameters:
        @display("i=smq");
    submodules:

        // Job publishing service
        pushServer: MessageQueueIn {
            localPort = 8444;
            messageQueueOutPath = "^.pullServer";
            @display("p=855.78204,74.514");
        }

		// Job consumtion service
        pullServer: MessageQueueOut {
            localPort = 8443;
            @display("p=652.6,78");
        }

    connections:

        // Connect the push server to transport layer
        pushServer.socketIn <-- at.out++;
        pushServer.socketOut --> at.in++;

        // Connect the pull server to transport layer
        pullServer.socketIn <-- at.out++;
        pullServer.socketOut --> at.in++;
}
