/**
 * @file EdgeHypervisor.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Module declaration for the Edge Hypervisor
 * @version 0.1
 * @date 2023-12-03
 *
 */
#ifndef SIMCAN_EX_EDGE_HYPERVISOR
#define SIMCAN_EX_EDGE_HYPERVISOR

#include "OperatingSystem/Hypervisors/common.h"
#include "Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.h"
#include "Management/dataClasses/NodeResourceRequest.h"
#include "Management/dataClasses/Application.h"
#include "Messages/SIMCAN_Message.h"
#include "cSIMCAN_Core.h"

using namespace hypervisor;

/**
 * @brief Class that models the behaviour of the EdgeNode hypervisor
 * @details It may be considered the OS of the EdgeNode
 */
class EdgeHypervisor : public cSIMCAN_Core
{
    // TODO : App timeouts ?
    // TODO : Evaluate if it belongs to a single user ? (It should)
protected:
    HardwareManager *hardwareManager;   // Currently not used
    AppControlBlock *appsControl;
    std::vector<uint32_t> freePids;     // Helps keep track of the available "slots" for the apps
    SystemSpecs hwSpecs;
    uint32_t maxApps;
    uint32_t runningApps;
    cModule *appsVector;

    void initialize();
    void finish();

    void launchApps(SM_UserAPP *request);
    void handleAppTermination(uint32_t pid, bool force);
    void handleIOAppRequest(uint32_t pid, bool completed);
    void handleSendRequest(uint32_t pid, bool completed);
    void handleBindAndListen(uint32_t pid);

    /**
     * Get the outGate ID to the module that sent <b>msg</b>
     * @param msg Arrived message.
     * @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
     */
    cGate *getOutGate(cMessage *msg);

    /**
     * Process a self message.
     * @param msg Self message.
     */
    void processSelfMessage(cMessage *msg);

    /**
     * Process a request message.
     * @param sm Request message.
     */
    void processRequestMessage(SIMCAN_Message *sm);

    /**
     * Process a response message.
     * @param sm Request message.
     */
    void processResponseMessage(SIMCAN_Message *sm);
};

#endif