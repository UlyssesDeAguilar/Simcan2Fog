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

#ifndef MANAGEMENT_PARSER_SLALISTPARSER_H_
#define MANAGEMENT_PARSER_SLALISTPARSER_H_

#include "Parser.h"
#include "Management/dataClasses/VirtualMachine.h"
#include "Management/dataClasses/Sla.h"

class SlaListParser: public Parser<Sla> {
public:
    /**
     * UserListParser constructor
     * @param s String to be parsed
     * @param vmTypesVector Vector of vm types to reference them
     * @param appTypesVector Vector of application types to reference them
     */
    SlaListParser(const char *s, std::vector<VirtualMachine*> *vmTypesVector);
    virtual ~SlaListParser();
    virtual int parse() override;

protected:
    int numSlas, currentSla;
    double costBase, costIncrease, costDiscount, costCompensation;
    int numVMs, currentVM;
    int result;
    Sla* currentSlaObject;
    VirtualMachine* vmPtr;
    std::string slaNameStr, slaTypeStr, vmNameStr;
    /** Vector that contains the types of VM used in the current simulation */
    std::vector<VirtualMachine*> *vmTypes;

    /**
     * Search for a specific type of VM.
     *
     * @param vmType Type of a VM.
     * @return If the requested type of VM is located in the vmTypes vector, then a pointer to its object is returned. In other case, a \a nullptr is returned.
     */
     VirtualMachine* findVirtualMachine (std::string vmType);
};

#endif /* MANAGEMENT_PARSER_SLALISTPARSER_H_ */
