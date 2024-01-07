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

#ifndef MANAGEMENT_PARSER_VMLISTPARSER_H_
#define MANAGEMENT_PARSER_VMLISTPARSER_H_

#include "Parser.h"
#include "Management/dataClasses/VirtualMachines/VirtualMachine.h"

class VmListParser: public Parser<VirtualMachine> {
public:
    /**
     * VmListParser constructor
     * @param s String to be parsed
     */
    VmListParser(const char *s);
    virtual ~VmListParser();
    virtual int parse() override;

protected:
    int numVMs, currentVm, numCores, result;
    double cost, scu, diskGB, memoryGB;
    VirtualMachine* currentVmObj;
    std::string instanceTypeStr;
    /** Vector that contains the types of VM used in the current simulation */
    std::vector<VirtualMachine*> vmTypes;
};

#endif /* MANAGEMENT_PARSER_VMLISTPARSER_H_ */
