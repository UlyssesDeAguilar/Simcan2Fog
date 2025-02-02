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

package s2f.apps.endpoints;

import s2f.apps.AppBase;

//
// Simple client application that acts as an initiating endpoint
//
simple ClientEndpoint extends AppBase
{
    parameters:
      	@class(ClientEndpoint);  
        string serviceName;		// Name of the service, used for HTTP/REST requests
        string servingHost;		// If numerical, it's the Ip of the endpoint. Otherwise a FQDN is expected
}
