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

#ifndef MANAGEMENT_PARSER_USERLISTPARSER_H_
#define MANAGEMENT_PARSER_USERLISTPARSER_H_

#include "Parser.h"
#include "Management/dataClasses/VirtualMachine.h"
#include "Management/dataClasses/CloudUser.h"
#include "Management/dataClasses/Sla.h"

class UserListParser: public Parser<CloudUser> {
public:
    /**
     * UserListParser constructor
     * @param s String to be parsed
     * @param vmTypesVector Vector of vm types to reference them
     * @param appTypesVector Vector of application types to reference them
     */
    UserListParser(const char *s, std::vector<VirtualMachine*> *vmTypesVector, std::vector<Application*> *appTypesVector);

    /**
     * Destructor
     */
    virtual ~UserListParser();
    virtual int parse() override;

protected:
    int numUsers, currentUser, numUserInstances;
    int numApps, currentApp, numAppInstances;
    int numVMs, currentVM, numVmInstances, nRentTime;
    int result;
    Application* appPtr;
    VirtualMachine* vmPtr;
    CloudUser* currentUserObject;
    std::string userTypeStr, appNameStr, vmNameStr;
    /** Pointer to the vector that contains the set of application types used in the current simulation */
    std::vector<Application*> *appTypes;
    /** Pointer to the vector that contains the types of VM used in the current simulation */
    std::vector<VirtualMachine*> *vmTypes;


    /**
     * Search for the application <b>appName</b>.
     *
     * @param appName Instance name of the requested application.
     * @return If the requested application is located in the vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
     */
     Application* findApplication (std::string appName);

    /**
     * Search for a specific type of VM.
     *
     * @param vmType Type of a VM.
     * @return If the requested type of VM is located in the vmTypes vector, then a pointer to its object is returned. In other case, a \a nullptr is returned.
     */
     VirtualMachine* findVirtualMachine (std::string vmType);
};

#endif /* MANAGEMENT_PARSER_USERLISTPARSER_H_ */
