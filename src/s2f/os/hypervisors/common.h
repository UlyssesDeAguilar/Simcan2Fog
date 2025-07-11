/**
 * @file common.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief This file contains common definitions and headers for all hypervisors
 * @version 0.1
 * @date 2023-12-05
 *
 */
#ifndef SIMCAN_EX_HYPERVISOR_COMMON_H_
#define SIMCAN_EX_HYPERVISOR_COMMON_H_

#include <deque>
#include <omnetpp.h>

#include "s2f/os/hardwaremanagers/HardwareManager.h"
#include "s2f/core/simdata/DataManager.h"
#include "s2f/messages/INET_AppMessage_m.h"
#include "s2f/messages/SM_CPU_Message.h"
#include "s2f/messages/SM_UserAPP.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/messages/SM_VmExtend_m.h"
#include "s2f/messages/Syscall_m.h"
#include "s2f/apps/ApplicationBuilder.h"

namespace s2f
{
    namespace os
    {

        typedef enum
        {
            VM_TIMEOUT, //!< A virtual machine reached the maximum renting time
            POWER_ON    //!< Event for powering on the machine
        } AutoEvent;
    }
}

#endif /* SIMCAN_EX_HYPERVISOR_COMMON_H_ */
