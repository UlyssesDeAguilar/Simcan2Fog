/*
 * SIMCAN2_types.h
 *
 *  Created on: 3 jul. 2019
 *      Author: a2b_8
 */

#ifndef _SIMCAN2_TYPES_H_
#define _SIMCAN2_TYPES_H_

#include <omnetpp.h>
#include <string>
using namespace omnetpp;

/************************* Typedefs *************************/

/** Typedef of priority types */
typedef enum{Regular, Priority} tPriorityType;

/** Labels of priority types */
static const char * tPriorityTypeLabel[] = {
    "Regular",
    "Priority"
};

#endif /* MANAGEMENT_DATACLASSES_SIMCAN2_TYPES_H_ */
