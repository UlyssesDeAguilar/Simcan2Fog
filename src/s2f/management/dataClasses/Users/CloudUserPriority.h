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

#ifndef MANAGEMENT_DATACLASSES_CLOUDUSERPRIORITY_H_
#define MANAGEMENT_DATACLASSES_CLOUDUSERPRIORITY_H_

#include "../../../management/dataClasses/Users/CloudUser.h"

class CloudUserPriority : public CloudUser
{

private:
    tPriorityType priorityType; // Priority type of the user requests in the cloud
    std::string sla;            //

public:
    CloudUserPriority(const std::string &type,
                      int numInstances,
                      tPriorityType priorityType,
                      const std::string &sla) : CloudUser(type, numInstances), priorityType(priorityType), sla(sla) {}

    /**
     * Gets the priority type assigned to this user.
     *
     * @return tPriorityType Priority type of the user requests in the cloud
     */
    tPriorityType getPriorityType() const { return priorityType; }

    /**
     * Assigns a priority type to this user.
     *
     * @param priorityType Priority type of the user requests in the cloud
     */
    void setPriorityType(tPriorityType priorityType) { this->priorityType = priorityType; }

    /**
     * Gets the sla assigned to this user.
     *
     * @return Sla signed by the user in the cloud
     */
    const std::string &getSla() const { return sla; }

    /**
     * Assigns a sla to this user.
     *
     * @param sla signed by the user in the cloud
     */
    void setSla(const std::string &sla) { this->sla = sla; }
};

#endif /* MANAGEMENT_DATACLASSES_CLOUDUSERPRIORITY_H_ */
