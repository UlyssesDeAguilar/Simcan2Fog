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

#include "VmListParser.h"

VmListParser::VmListParser(const char *s) : Parser<VirtualMachine>(s) {
    // Init...
    result = SC_OK;
    currentVm = 0;
}

VmListParser::~VmListParser() {
    // TODO Auto-generated destructor stub
}

int VmListParser::parse() {
            cStringTokenizer tokenizer(parseString);

            // First token, number of VMs
            if (tokenizer.hasMoreTokens()){
                const char *numVMsChr = tokenizer.nextToken();
                numVMs = atoi(numVMsChr);
            }
            else{
                EV_ERROR << "Cannot read the first token (number of VMs)" << endl;
                result = SC_ERROR;
            }

            // While there are unprocessed items...
            while ((tokenizer.hasMoreTokens()) && (currentVm < numVMs) && (result==SC_OK)){

                // Get the VM type
                if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                  const char *vmTypeChr = tokenizer.nextToken();
                  instanceTypeStr = vmTypeChr;
                }
                else{
                  EV_ERROR << "Cannot read the type of VM:" << currentVm << endl;
                  result = SC_ERROR;
                }


                // Get the Cost
                if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                    const char *costChr = tokenizer.nextToken();
                    cost = atof(costChr);
                }
                else{
                    EV_ERROR << "Cannot read the cost of VM:" << currentVm << endl;
                    result = SC_ERROR;
                }

                // Get the number of cores
                if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                    const char *numCoresChr = tokenizer.nextToken();
                    numCores = atoi (numCoresChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of cores of VM:" << currentVm << endl;
                    result = SC_ERROR;
                }

                // Get the SCU
                if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                    const char *scuChr = tokenizer.nextToken();
                    scu = atof (scuChr);
                }
                else{
                    EV_ERROR << "Cannot read the SCU of VM:" << currentVm << endl;
                    result = SC_ERROR;
                }

                // Get the DiskGB
                if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                    const char *diskChr = tokenizer.nextToken();
                    diskGB = atof (diskChr);
                }
                else{
                    EV_ERROR << "Cannot read the amount of disk space of VM:" << currentVm << endl;
                    result = SC_ERROR;
                }

                // Get the memoryGB
                if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
                   const char *memChr = tokenizer.nextToken();
                   memoryGB = atof (memChr);
                }
                else{
                   EV_ERROR << "Cannot read the amount of memory space of VM:" << currentVm << endl;
                   result = SC_ERROR;
                }

                // Create current VM instance
                currentVmObj = new VirtualMachine (instanceTypeStr, cost, numCores, scu, diskGB, memoryGB);

                // Insert current VM in the vector
                parseResult.push_back(currentVmObj);

                // Process next VM
                currentVm++;
            }


       return result;
}

