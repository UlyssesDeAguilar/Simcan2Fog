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
import inet.applications.contract.IApp;
import inet.applications.tcpapp.TcpClientSocketIo;

module QueuePushClient like IApp
{
    parameters:
        string connectAddress;
        int connectPort = default(8444);
        
    gates:
        input socketIn @labels(ITransportPacket/up);
        output socketOut @labels(ITransportPacket/down);
        input queueIn;
    
    submodules:
        service: PushClient;
        socket: TcpClientSocketIo{
            connectAddress = connectAddress;
            connectPort = connectPort;
        }
        
    connections:
        service.netOut --> socket.trafficIn;
        service.netIn <-- socket.trafficOut;
        
        service.queueIn <-- queueIn;
        socket.socketIn <-- socketIn;
        socket.socketOut --> socketOut;
}
