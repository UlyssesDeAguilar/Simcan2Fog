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

#include "SlaListParser.h"

SlaListParser::SlaListParser(const char *s, std::vector<VirtualMachine*> *vmTypesVector) : Parser<Sla>(s) {
    vmTypes = vmTypesVector;
    // Init...
    result = SC_OK;
    numSlas = currentSla = 0;
    costBase = costIncrease = costDiscount = costCompensation = 0.0;
    numVMs = currentVM = 0;
}

SlaListParser::~SlaListParser() {

}

int SlaListParser::parse() {
        cStringTokenizer tokenizer(parseString);

        // First token, number of users
        if (tokenizer.hasMoreTokens()){
            const char *numSlasChr = tokenizer.nextToken();
            numSlas = atoi(numSlasChr);
        }
        else{
            EV_ERROR << "Cannot read the first token (number of users)" << endl;
            result = SC_ERROR;
        }


        // For each user in the list...
        while ((tokenizer.hasMoreTokens()) && (currentSla < numSlas) && (result==SC_OK)){

            // Get the user type
            if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                const char *slaTypeChr = tokenizer.nextToken();
                slaTypeStr = slaTypeChr;
            }
            else{
                EV_ERROR << "Cannot read the sla name for sla:" << currentSla << endl;
                result = SC_ERROR;
            }

                // Get the number of VMs for the current sla
                if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                    const char *numVMsChr = tokenizer.nextToken();
                    numVMs = atoi(numVMsChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of VMs for sla:" << slaTypeStr << endl;
                    result = SC_ERROR;
                }


                // Parsing VMs for current user...
                if (result == SC_OK){

                    // Init...
                    currentVM = 0;

                    // Create current sla instance
                    currentSlaObject = new Sla(slaTypeStr);

                    // Include each VMs
                    while ((currentVM<numVMs) && (result==SC_OK)){

                        // Get the VM name
                        if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                            const char *vmNameChr = tokenizer.nextToken();
                            vmNameStr = vmNameChr;
                        }
                        else{
                            EV_ERROR << "Cannot read the VM name[" << currentVM << "] for sla:" << slaTypeStr << endl;
                            result = SC_ERROR;
                        }

                        // Get the VM base cost
                        if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                            const char *costBaseChr = tokenizer.nextToken();
                            costBase = atof(costBaseChr);
                        }
                        else{
                            EV_ERROR << "Cannot read the number of instances for VM [" << currentVM << "] in sla:" << slaTypeStr << endl;
                            result = SC_ERROR;
                        }

                        // Get the VM cost increase
                        if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                            const char *costIncreaseChr = tokenizer.nextToken();
                            costIncrease = atof(costIncreaseChr);
                        }
                        else{
                            EV_ERROR << "Cannot read the number of instances for VM [" << currentVM << "] in sla:" << slaTypeStr << endl;
                            result = SC_ERROR;
                        }

                        // Get the VM cost discount
                        if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                            const char *costDiscountChr = tokenizer.nextToken();
                            costDiscount = atof(costDiscountChr);
                        }
                        else{
                            EV_ERROR << "Cannot read the number of instances for VM [" << currentVM << "] in sla:" << slaTypeStr << endl;
                            result = SC_ERROR;
                        }

                        // Get the VM cost compensation
                        if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                            const char *costCompensationChr = tokenizer.nextToken();
                            costCompensation = atof(costCompensationChr);
                        }
                        else{
                            EV_ERROR << "Cannot read the number of instances for VM [" << currentVM << "] in sla:" << slaTypeStr << endl;
                            result = SC_ERROR;
                        }

                        // Locate the VM in the vector
                        vmPtr = findVirtualMachine (vmNameStr);

                            // Vm found!
                            if (vmPtr != nullptr){
                                currentSlaObject->addVmCost(vmPtr, costBase, costIncrease, costDiscount, costCompensation);;
                            }
                            else{
                                EV_ERROR << "Virtual Machine not found [" << vmNameStr << "] in sla:" << slaTypeStr << endl;
                                result = SC_ERROR;
                            }

                        // Next vm
                        currentVM++;
                    }
                }

               // Include sla in the vector
                parseResult.push_back(currentSlaObject);

                // Process next sla
                currentSla++;
        }

    return result;
}

VirtualMachine* SlaListParser::findVirtualMachine (std::string vmType){

    std::vector<VirtualMachine*>::iterator it;
    VirtualMachine* result;
    bool found;

        // Init
        found = false;
        result = nullptr;
        it = vmTypes->begin();

        // Search...
        while((!found) && (it != vmTypes->end())){

            if ((*it)->getType() == vmType){
                found = true;
                result = (*it);
            }
            else
                it++;
        }

    return result;
}

