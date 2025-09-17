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

simple P2PBase extends AppBase {
	parameters:
	    @class(P2PBase);
		int listeningPort;
		int peerRequestLimit = default(3);
		string dnsSeed @mutable;
		string localIp @mutable;
		string dnsIp @mutable;
		string testIp @mutable;
}
