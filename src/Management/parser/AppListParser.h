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

#ifndef MANAGEMENT_PARSER_APPLISTPARSER_H_
#define MANAGEMENT_PARSER_APPLISTPARSER_H_

#include "Parser.h"
#include "Management/dataClasses/Applications/Application.h"

// FIXME: Update this class, won't work with the new Application parameters !
class AppListParser: public Parser<Application> {
public:
    /**
     * AppListParser constructor
     * @param s String to be parsed
     */
    AppListParser(const char *s);
    virtual ~AppListParser();
    virtual int parse() override;

protected:
    int numApplications, currentApp, numParams, currentParam;
    int result;
    tNedType paramTypeNed;
    Application* currentAppObj;
    AppParameter* currentParameterObj;
    std::string instanceNameStr, instanceTypeStr, paramNameStr, paramTypeStr, paramValueStr;

};

#endif /* MANAGEMENT_PARSER_APPLISTPARSER_H_ */
