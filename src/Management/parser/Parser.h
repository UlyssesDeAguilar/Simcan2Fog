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

#ifndef MANAGEMENT_PARSER_PARSER_H_
#define MANAGEMENT_PARSER_PARSER_H_

#include "Core/cSIMCAN_Core.h"
#include "Management/dataClasses/include/SIMCAN2_types.h"

template <class T>
class Parser {
public:
    /**
     * Abstract parser constructor
     * @param s String to be parsed
     */
    Parser(const char *s)
    {
        this->parseString = s;
    }

    /**
     * Abstract parser destructor
     */
    virtual ~Parser()
    {
        parseResult.clear();
    }

    /**
     * Abstract function which shall be implemented.
     * Parses each object type used in the simulation. These objects shall be allocated in the <b>parseResult</b> vector.
     *
     * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
     */
    virtual int parse() = 0;

    /**
     * Gets the result of the parse process
     * @return vector<T> Vector of parsed T objects
     */
    virtual std::vector<T *> getResult()
    {
        return parseResult;
    }

protected:

    /** String to be parsed */
    const char *parseString;

    /** Result obtained after parse method execution */
    std::vector<T *> parseResult;
};

#endif /* MANAGEMENT_PARSER_PARSER_H_ */
