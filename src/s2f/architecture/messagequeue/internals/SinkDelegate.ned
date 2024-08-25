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

package s2f.architecture.messagequeue.internals;
import inet.queueing.sink.ActivePacketSink;

simple SinkDelegate extends ActivePacketSink
{
    parameters:
        collectionInterval @unit(s) = default(5s);   // Default check for queue contents
        @class(SinkDelegate);    
    gates:
        output out @directIn;
}
