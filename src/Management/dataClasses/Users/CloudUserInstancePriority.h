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

#ifndef MANAGEMENT_DATACLASSES_CLOUDUSERINSTANCEPRIORITY_H_
#define MANAGEMENT_DATACLASSES_CLOUDUSERINSTANCEPRIORITY_H_

#include "CloudUserInstance.h"

class CloudUserInstancePriority: public CloudUserInstance {
    public:
        CloudUserInstancePriority(const CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances);
        virtual ~CloudUserInstancePriority();
    bool isBCredit() const;
    void setBCredit(bool bCredit);

    private:
        bool bCredit;
};

#endif /* MANAGEMENT_DATACLASSES_CLOUDUSERINSTANCEPRIORITY_H_ */
