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

package s2f.architecture.net.stack.queue;

import inet.applications.tcpapp.TcpClientSocketIo;
import inet.common.MessageDispatcher;


module QueueClient
{
    parameters:
        string connectAddress;
        string topic;
        int pullPort = default(8443);
		int pushPort = default(8444);
        @display("bgb=159.76334,115.97634");
        
        pushSocket.connectAddress = this.connectAddress;
        pullSocket.connectAddress = this.connectAddress;
        pushSocket.connectPort = this.pushPort;
        pullSocket.connectPort = this.pullPort;
        
        pullService.topic = this.topic;
        pushService.topic = this.topic;
        
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
        input queueIn;
        output queueOut;

    submodules:
        pullService: PullClient {
            @display("p=117.751495,21.301777");
        }
        pullSocket: TcpClientSocketIo {
            @display("p=117.751495,59.76332");
        }

        pushService: PushClient {
            @display("p=48.520714,21.301777");
        }
        pushSocket: TcpClientSocketIo {
            @display("p=48.520714,59.76332");
        }

        qt: MessageDispatcher {
            @display("p=80,95;b=,,rect");
        }
    connections:
        pullService.netOut --> pullSocket.trafficIn;
        pullService.netIn <-- pullSocket.trafficOut;
        pushService.netOut --> pushSocket.trafficIn;
        pushService.netIn <-- pushSocket.trafficOut;

        pushSocket.socketOut --> qt.in++;
        pushSocket.socketIn <-- qt.out++;
        pullSocket.socketOut --> qt.in++;
        pullSocket.socketIn <-- qt.out++;

        pullService.queueOut --> queueOut;
        pushService.queueIn <-- queueIn;
        qt.out++ --> socketOut;
        qt.in++ <-- socketIn;
}
