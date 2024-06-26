/*
 * LogUtils.cpp
 *
 *  Created on: 2 jul. 2020
 *      Author: Mowser
 */

#include "../../management/utils/LogUtils.h"

#include <string.h>

LogUtils::LogUtils() {
    // TODO Auto-generated constructor stub

}

LogUtils::~LogUtils() {
    // TODO Auto-generated destructor stub
}

std::string LogUtils::prettyFunc(const char *fileName, const char *funcName)
{
    const char *lastBar;

    char *className;

    int startPos,
        length;

    std::string strFile,
                prettyName;

    lastBar = strrchr(fileName, '/');
    if (lastBar == nullptr)
        lastBar = strrchr(fileName, '\\');

    startPos = lastBar - fileName + 1;
    length = strrchr(fileName, '.') - fileName - startPos;
    strFile = fileName;
    prettyName = strFile.substr(startPos, length) + "::" + funcName;

    return prettyName;
}
