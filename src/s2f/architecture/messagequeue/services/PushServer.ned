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

package s2f.architecture.messagequeue.services;
import inet.applications.tcpapp.TcpServerSocketIo;
import inet.applications.contract.IApp;

module PushServer like IApp
{
    parameters:
        @display("i=block/app;bgb=302,411");
    gates:
        input socketIn;
        output socketOut;
    submodules:
        pushService: PushService {

            @display("p=151,101");
        }
        io: TcpServerSocketIo {
            parameters:
                @display("p=151,238");
        }
    connections:
        // Connect socket to the application
        io.trafficOut --> pushService.in;
        pushService.out --> io.trafficIn;

        // Connect socket to the transport layer
        io.socketOut --> { @display("m=s"); } --> socketOut;
        socketIn --> { @display("m=s"); } --> io.socketIn;
}
