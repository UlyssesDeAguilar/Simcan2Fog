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

package s2f.apps.p2p;
import s2f.apps.AppBase;

simple P2PApplication extends AppBase {
	parameters:
	    @class(P2PApplication);
		string parentPath = default("^.^.^.^.");
		int processingMIs = default(1);						// not needed for now
		int listeningPort = default(8333);					// mainnet port	
        int synthSize @unit(B) = default(40B);              // The payload that'll be sent
        int pingsThreshold = default(5);                    // The number of pings where it will start processing
        string localInterface = default("");
        string externalInterface = default("");
        string dnsSeed @mutable;
        double pollingInterval @unit(s) = default(5s);
        
        // These signals are only registering the output and input traffic from the endpoint
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @signal[roundTripTime](type=double);

        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        @statistic[roundTripTime](title="round trip time"; unit=s; record=histogram,vector; interpolationmode=none);
		
}