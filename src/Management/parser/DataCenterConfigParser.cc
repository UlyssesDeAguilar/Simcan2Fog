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

#include "DataCenterConfigParser.h"

DataCenterConfigParser::DataCenterConfigParser(const char *s) : Parser<DataCenter>(s) {
    result = SC_OK;
}

DataCenterConfigParser::~DataCenterConfigParser() {
    // TODO Auto-generated destructor stub
}


int DataCenterConfigParser::parse() {
        cStringTokenizer tokenizer(parseString);

        // Get the Data-Center name
        if ((tokenizer.hasMoreTokens()) && (result==SC_OK)){
          const char *dataCenterNameChr = tokenizer.nextToken();
          dataCenterNameStr = dataCenterNameChr;
        }
        else{
          EV_ERROR << "Cannot read name of Data-Center" << endl;
          result = SC_ERROR;
        }

        // Create a new Data-Center Info
        if (result==SC_OK){

            // Init...
            currentRack = numComputingRacks = numStorageRacks = 0;

            // Create data-Center object
            dataCenterObj = new DataCenter (dataCenterNameStr);

            // Parse #computingRacks
            if (tokenizer.hasMoreTokens()){
                const char *numComputingRacksChr = tokenizer.nextToken();
                numComputingRacks = atoi(numComputingRacksChr);
            }
            else{
                EV_ERROR << "Cannot read the the number of Computing Racks for data-center:"  << dataCenterNameStr  << endl;
                result = SC_ERROR;
            }

            // Parse current rack info...
            while ((tokenizer.hasMoreTokens()) && (currentRack < numComputingRacks) && (result==SC_OK)){

                // Init...
                numRackInstances = 0;
                rackInfo = nullptr;
                nodeInfo = nullptr;

                // Number of rack instances
                if (tokenizer.hasMoreTokens() && (result==SC_OK) ){
                    const char *numRackInstancesChr = tokenizer.nextToken();
                    numRackInstances = atoi(numRackInstancesChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of instances for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Rack Name
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *rackNameChr = tokenizer.nextToken();
                    rackName = rackNameChr;
                }
                else{
                    EV_ERROR << "Cannot read the rack name for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Num boards
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *numBoardsChr = tokenizer.nextToken();
                    numBoards = atoi(numBoardsChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of boards for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Blades per board
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *bladesPerBoardChr = tokenizer.nextToken();
                    bladesPerBoard = atoi(bladesPerBoardChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of blades per board for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Node Name
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *nodeNameChr = tokenizer.nextToken();
                    nodeName = nodeNameChr;
                }
                else{
                    EV_ERROR << "Cannot read the node name for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Disk
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *diskChr = tokenizer.nextToken();
                    disk = atoi(diskChr);
                }
                else{
                    EV_ERROR << "Cannot read the Disk for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Memory
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *memoryChr = tokenizer.nextToken();
                    memory = atof(memoryChr);
                }
                else{
                    EV_ERROR << "Cannot read the Memory for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // CPU (NumCores)
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *numCpusChr = tokenizer.nextToken();
                    numCpus = atoi(numCpusChr);
                }
                else{
                    EV_ERROR << "Cannot read the #Cpus for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // CPU (Speed)
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *cpuSpeedChr = tokenizer.nextToken();
                    cpuSpeed = atoi(cpuSpeedChr);
                }
                else{
                    EV_ERROR << "Cannot read the CPU speed for the computing rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Generate Racks and include them in the Data-Center object!

                    // Is the current Rack info already loaded?
                    rackInfo = nullptr;
                    nodeInfo = nullptr;
                    //rackInfo = findRackInfo (rackName, false);
                    //nodeInfo = findNodeInfo (nodeName, false);

                    // Rack info not found... create a new one!
                    if (rackInfo == nullptr){

                       // Node info not found... create a new one!
                       if (nodeInfo == nullptr){
                           nodeInfo = new NodeInfo (nodeName, false, disk, memory, numCpus, cpuSpeed);
                       }

                        rackInfo = new RackInfo(rackName, false, numBoards, bladesPerBoard, nodeInfo);
                    }
                    // Check!
                    else if (nodeInfo == nullptr){
                            //error ("Node info cannot be nullptr! Error while parsing dataCenter %s - Rack %s - Node %s", dataCenterNameStr.c_str(), rackName.c_str(), nodeName.c_str());
                    }


                    // Create rack instances...
                    for (currentRackInstance = 0; currentRackInstance < numRackInstances; currentRackInstance++){

                        // Create the Rack object
                        rack = new Rack (rackInfo);

                        // Add current rack to the data-center
                        dataCenterObj->addRack(rack, false);
                    }

                // Process next rack
                currentRack++;
            }

            // ------------ End of parsing [Computing Racks] ------------

            // Init...
            currentRack = 0;

            // Parse #storageRacks
            if (tokenizer.hasMoreTokens()){
                const char *numStorageRacksChr = tokenizer.nextToken();
                numStorageRacks = atoi(numStorageRacksChr);
            }
            else{
                EV_ERROR << "Cannot read the the number of Storage Racks for data-center:"  << dataCenterNameStr  << endl;
                result = SC_ERROR;
            }

            // Parse current rack info...
            while ((tokenizer.hasMoreTokens()) && (currentRack < numStorageRacks) && (result==SC_OK)){

                // Init...
                numRackInstances = 0;
                rackInfo = nullptr;
                nodeInfo = nullptr;

                // Number of rack instances
                if (tokenizer.hasMoreTokens() && (result==SC_OK) ){
                    const char *numRackInstancesChr = tokenizer.nextToken();
                    numRackInstances = atoi(numRackInstancesChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of instances for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Rack Name
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *rackNameChr = tokenizer.nextToken();
                    rackName = rackNameChr;
                }
                else{
                    EV_ERROR << "Cannot read the rack name for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Num boards
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *numBoardsChr = tokenizer.nextToken();
                    numBoards = atoi(numBoardsChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of boards for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Blades per board
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *bladesPerBoardChr = tokenizer.nextToken();
                    bladesPerBoard = atoi(bladesPerBoardChr);
                }
                else{
                    EV_ERROR << "Cannot read the number of blades per board for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Node Name
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *nodeNameChr = tokenizer.nextToken();
                    nodeName = nodeNameChr;
                }
                else{
                    EV_ERROR << "Cannot read the node name for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Disk
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *diskChr = tokenizer.nextToken();
                    disk = atoi(diskChr);
                }
                else{
                    EV_ERROR << "Cannot read the Disk for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Memory
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *memoryChr = tokenizer.nextToken();
                    memory = atof(memoryChr);
                }
                else{
                    EV_ERROR << "Cannot read the Memory for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // CPU (NumCores)
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *numCpusChr = tokenizer.nextToken();
                    numCpus = atoi(numCpusChr);
                }
                else{
                    EV_ERROR << "Cannot read the #Cpus for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // CPU (Speed)
                if (tokenizer.hasMoreTokens() && (result==SC_OK)){
                    const char *cpuSpeedChr = tokenizer.nextToken();
                    cpuSpeed = atoi(cpuSpeedChr);
                }
                else{
                    EV_ERROR << "Cannot read the CPU speed for the storage rack ["  << currentRack  << "]" << endl;
                    result = SC_ERROR;
                }

                // Generate Racks and include them in the Data-Center object!

                    // Is the current Rack info already loaded?
                    rackInfo = nullptr;
                    nodeInfo = nullptr;
                    //rackInfo = findRackInfo (rackName, true);
                    //nodeInfo = findNodeInfo (nodeName, true);

                    // Rack info not found... create a new one!
                    if (rackInfo == nullptr){

                       // Node info not found... create a new one!
                       if (nodeInfo == nullptr){
                           nodeInfo = new NodeInfo (nodeName, true, disk, memory, numCpus, cpuSpeed);
                       }

                        rackInfo = new RackInfo(rackName, true, numBoards, bladesPerBoard, nodeInfo);
                    }
                    // Check!
                    else if (nodeInfo == nullptr){
                            //error ("Node info cannot be nullptr! Error while parsing dataCenter %s - Rack %s - Node %s", dataCenterNameStr.c_str(), rackName.c_str(), nodeName.c_str());
                    }


                    // Create rack instances...
                    for (currentRackInstance = 0; currentRackInstance < numRackInstances; currentRackInstance++){

                        // Create the Rack object
                        rack = new Rack (rackInfo);

                        // Add current rack to the data-center
                        dataCenterObj->addRack(rack, true);
                    }

                // Process next rack
                currentRack++;
            }

            // ------------ End of parsing [Storage Racks] ------------
        }
        // Insert the new Data-Center into the vector
        parseResult.push_back(dataCenterObj);

   return result;
}
