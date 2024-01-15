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

#include "AppListParser.h"

AppListParser::AppListParser(const char *s) : Parser<Application>(s)
{
    // Init...
    result = SC_OK;
    currentApp = currentParam = 0;
}

AppListParser::~AppListParser()
{
}

int AppListParser::parse()
{
    cStringTokenizer tokenizer(parseString);

    // First token, number of applications
    if (tokenizer.hasMoreTokens())
    {
        const char *numAppsChr = tokenizer.nextToken();
        numApplications = atoi(numAppsChr);
    }
    else
    {
        EV_ERROR << "Cannot read the first token (number of applications)" << endl;
        result = SC_ERROR;
    }

    // While there are unprocessed items...
    while ((tokenizer.hasMoreTokens()) && (currentApp < numApplications) && (result == SC_OK))
    {

        // Get the instance type
        if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
        {
            const char *instanceTypeChr = tokenizer.nextToken();
            instanceTypeStr = instanceTypeChr;
        }
        else
        {
            EV_ERROR << "Cannot read instance type for application:" << currentApp << endl;
            result = SC_ERROR;
        }

        // Get the instance name
        if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
        {
            const char *instanceNameChr = tokenizer.nextToken();
            instanceNameStr = instanceNameChr;
        }
        else
        {
            EV_ERROR << "Cannot read instance name for application:" << currentApp << endl;
            result = SC_ERROR;
        }

        // FIXME:: READ THE APP PACKAGE HERE -- This fix behaves like the original method in DC manager Base handleUserAppRequest()
        std::string package =  "simcan2.Applications.UserApps." + instanceTypeStr + "." +  instanceTypeStr;

        // Get the number of parameters
        if ((tokenizer.hasMoreTokens()) && (result == SC_OK))
        {
            const char *numParamsChr = tokenizer.nextToken();
            numParams = atoi(numParamsChr);
        }
        else
        {
            EV_ERROR << "Cannot read number of parameters for application:" << currentApp << endl;
            result = SC_ERROR;
        }

        // Name, type and params have been read OK! Creating AppInstance object...
        if (result == SC_OK)
        {

            // New AppInstance
            currentAppObj = new Application(instanceNameStr,instanceTypeStr, package);
            currentParam = 0;

            // Read app parameters...
            while ((currentParam < numParams) && (result == SC_OK))
            {

                // Get the parameter name
                if (tokenizer.hasMoreTokens())
                {
                    const char *paramNameChr = tokenizer.nextToken();
                    paramNameStr = paramNameChr;
                }
                else
                {
                    EV_ERROR << "Cannot read parameter name[" << currentParam << "] for application:" << currentApp << endl;
                    result = SC_ERROR;
                }

                // Get the parameter type
                if (tokenizer.hasMoreTokens())
                {
                    const char *paramTypeChr = tokenizer.nextToken();
                    paramTypeStr = paramTypeChr;
                }
                else
                {
                    EV_ERROR << "Cannot read parameter type[" << currentParam << "] for application:" << currentApp << endl;
                    result = SC_ERROR;
                }

                // Get the parameter value and unit
                if (tokenizer.hasMoreTokens())
                {
                    const char *paramValueChar = tokenizer.nextToken();
                    paramValueStr = paramValueChar;
                }
                else
                {
                    EV_ERROR << "Cannot read parameter value and unit[" << currentParam << "] for application:" << currentApp << endl;
                    result = SC_ERROR;
                }

                // Create a parameter object FIXME: DEFAULT UNIT - is a problem
                currentParameterObj = new AppParameter(paramNameStr, paramTypeStr, paramValueStr);

                // Set values...
                currentAppObj->insertParameter(currentParameterObj);

                // Process next parameter
                currentParam++;
            }

            // Insert current app in the vector
            parseResult.push_back(currentAppObj);
        }
    }

    return result;
}
