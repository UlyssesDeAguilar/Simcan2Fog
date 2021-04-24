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

#ifndef MANAGEMENT_PARSER_DATACENTRELISTPARSER_H_
#define MANAGEMENT_PARSER_DATACENTRELISTPARSER_H_

#include "Parser.h"
#include "Management/dataClasses/DataCentre.h"

class DataCentreListParser: public Parser<DataCentre> {
public:
    DataCentreListParser(const char *s);
    virtual ~DataCentreListParser();
    virtual int parse() override;

protected:
    int numDataCentres, currentDataCentre, result;
    int numComputingRacks, numStorageRacks, currentRack, numRackInstances, currentRackInstance;
    std::string dataCentreNameStr;
    std::string rackName, nodeName;
    int numBoards, bladesPerBoard;
    int disk, numCpus, cpuSpeed;
    double memory;
    DataCentre* dataCentreObj;
    RackInfo* rackInfo;
    NodeInfo* nodeInfo;
    Rack* rack;
};

#endif /* MANAGEMENT_PARSER_DATACENTRELISTPARSER_H_ */
