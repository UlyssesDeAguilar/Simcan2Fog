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

import s2f.messages.SM_UserVM;

//
// Message for negotiating a VM deployment between the user and the CloudProvider.
// It takes into account the user type so the CloudProvider can make decisions
// based upon the priority of said user.
//
// @author Pablo C. Cañizares
// @version 1.0
message SM_UserVM_Cost extends SM_UserVM {
    string strUserType; // The user type
    bool bPriorized; // Whether this request should be prioritized
}
