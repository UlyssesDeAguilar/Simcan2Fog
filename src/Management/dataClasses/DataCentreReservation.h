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

#ifndef MANAGEMENT_DATACLASSES_DATACENTRERESERVATION_H_
#define MANAGEMENT_DATACLASSES_DATACENTRERESERVATION_H_

#include "DataCentre.h"

class DataCentreReservation: public DataCentre {
public:
    DataCentreReservation(std::string name, int nReservedNodes);
    virtual ~DataCentreReservation();
    int getReservedNodes() const;
    void setReservedNodes(int reservedNodes);

protected:
    int nReservedNodes;
};

#endif /* MANAGEMENT_DATACLASSES_DATACENTRERESERVATION_H_ */
