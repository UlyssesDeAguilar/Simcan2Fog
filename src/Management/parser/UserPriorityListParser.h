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

#ifndef MANAGEMENT_PARSER_USERPRIORITYLISTPARSER_H_
#define MANAGEMENT_PARSER_USERPRIORITYLISTPARSER_H_

#include "UserListParser.h"
#include "Management/dataClasses/CloudUserPriority.h"

class UserPriorityListParser: public UserListParser {
protected:
    std::string slaTypeStr, priorityTypeStr;
    Sla* slaPtr;
    tPriorityType priorityType;
    /** Pointer to the vector that contains the types of sla used in the current simulation */
    std::vector<Sla*> *slaTypes;

    /**
     * Search for a specific type of Sla.
     *
     * @param slaType Type of a Sla.
     * @return If the requested type of Sla is located in the slaTypes vector, then a pointer to its object is returned. In other case, a \a nullptr is returned.
     */
    Sla* findSla (std::string slaType);


public:
    UserPriorityListParser(const char *s, std::vector<VirtualMachine*> *vmTypesVector, std::vector<Application*> *appTypesVector, std::vector<Sla*> *slaTypesVector);
    virtual ~UserPriorityListParser();
    virtual int parse() override;
};

#endif /* MANAGEMENT_PARSER_USERPRIORITYLISTPARSER_H_ */
