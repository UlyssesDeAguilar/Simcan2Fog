/*
 * LogUtils.h
 *
 *  Created on: 2 jul. 2020
 *      Author: Mowser
 */

#ifndef MANAGEMENT_UTILS_LOGUTILS_H_
#define MANAGEMENT_UTILS_LOGUTILS_H_

#include <string>

class LogUtils {
public:
    LogUtils();
    virtual ~LogUtils();

    /** Log message */
    static std::string prettyFunc(const char *fileName, const char *funcName);
};

#endif /* MANAGEMENT_UTILS_LOGUTILS_H_ */
