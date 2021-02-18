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

#ifndef MANAGEMENT_PARSER_DATACENTERRESERVATIONCONFIGPARSER_H_
#define MANAGEMENT_PARSER_DATACENTERRESERVATIONCONFIGPARSER_H_

#include "DataCenterConfigParser.h"
#include "Management/dataClasses/DataCenterReservation.h"

class DataCenterReservationConfigParser: public DataCenterConfigParser {
public:
    DataCenterReservationConfigParser(const char *s);
    virtual ~DataCenterReservationConfigParser();
    virtual int parse() override;

protected:
    DataCenterReservation* dataCenterReservationObj;
    int nReservedNodes;
};
#endif /* MANAGEMENT_PARSER_DATACENTERCONFIGPARSER_H_ */
