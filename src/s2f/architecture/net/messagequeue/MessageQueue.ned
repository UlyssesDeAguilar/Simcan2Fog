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

package s2f.Architecture.Network.MessageQueue;

import s2f.Architecture.Network.MessageQueue.MessageQueueManager;
import s2f.Architecture.Network.MessageQueue.SMQueue;
import inet.applications.tcpapp.TcpServerApp;
import inet.queueing.buffer.PacketBuffer;
import inet.node.inet.StandardHost;


module MessageQueue extends StandardHost
{
    parameters:
        @display("i=smq");
    submodules:
        // The main manager, creates and resolves queues for services
        manager: MessageQueueManager {

            @display("p=899.32495,75.6");
        }
        // There will be dynamic allocation of queues
        queues[0]: SMQueue {
            @display("p=1015.3,78");
        }

        // Helps optimizing storage across multiple queues
        buffer: PacketBuffer {
            @display("p=1123.2,79.299995");
        }

        // Job publishing service
        pushServer: TcpServerApp {
            listener.localPort = 8444;
            listener.serverConnectionModuleType = "s2f.Architecture.Network.MessageQueue.PushServer";
            @display("p=785.2,79.299995");
        }

		// Job consumtion service
        pullServer: TcpServerApp {
            listener.localPort = 8443;
            listener.serverConnectionModuleType = "s2f.Architecture.Network.MessageQueue.PullServer";
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
