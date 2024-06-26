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
import s2f.Architecture.Network.MessageQueue.Services.PullService;
import inet.applications.tcpapp.TcpServerSocketIo;
import inet.applications.contract.IApp;

module PullServer like IApp
{
    parameters:
        @display("i=block/app;bgb=355,300");
    gates:
        input socketIn;
        output socketOut;
    submodules:
        pullService: PullService {
            @display("p=175,66");
        }
        
        io: TcpServerSocketIo {
            parameters:
                @display("p=175,206");
        }
    connections:
        // Connect socket to the application
        io.trafficOut --> pullService.in;
        pullService.out --> io.trafficIn;

        // Connect socket to the transport layer
        io.socketOut --> { @display("m=s"); } --> socketOut;
        socketIn --> { @display("m=s"); } --> io.socketIn;
}
