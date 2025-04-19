#ifndef SIMCAN_EX_CONTROL_ENTRIES_H__
#define SIMCAN_EX_CONTROL_ENTRIES_H__

#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"
#include "s2f/messages/SM_CPU_Message.h"
#include "s2f/messages/SM_UserAPP.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/messages/SM_VmExtend_m.h"
#include "s2f/messages/Syscall_m.h"

/**
 * @brief Keeps all the necessary control information for the app
 */
struct AppControlBlock
{
    int pid = -1;             //!< Process Id, -1 if not allocated
    int vmId = -1;            //!< Group Id -- Will help to identify the VM
    int deploymentIndex = -1; //!< The index relative for the request that instantiated this app
    // tApplicationState status; //!< The exit status (0 - OK, 1 - ERROR, 2 - FORCED_EXIT, 3 - RUNNING)
    // std::map<int, ConnectionMode> sockets; //!< Map that contains the application sockets

    void initialize(int vmId, int pid, int deploymentIndex)
    {
        this->pid = pid;
        this->vmId = vmId;
        this->deploymentIndex = deploymentIndex;
    }

    void free() { this->pid = -1; }
    bool isFree() { return pid == -1; }
};

/**
 * @brief Keeps all the necessary control information for the VM
 */
struct VmControlBlock
{
    int vmId;                          //!< The vmId
    tVmState state = tVmState::vmIdle; //!< The current state of the VM
    const VirtualMachine *vmType{};    //!< The virtual machine type
    opp_string userId;                 //!< The current owner of the vm
    std::vector<AppControlBlock> apps; //!< The apps that are currently executing
    cQueue callBuffer;                 //!< Holds system calls when the vm is in suspension state
    SM_UserAPP *request{};             //!< The request that instantiated the app

    ~VmControlBlock();
    void initialize(int vmId, const VirtualMachine *vmType, const char *userId, size_t apps = 0);
    void resize(size_t apps);
    void free();

    void setRunning() { state = tVmState::vmRunning; }
    void setSuspended() { state = tVmState::vmSuspended; }
    bool isRunning() { return state == tVmState::vmRunning; }
    bool isSuspended() { return state == tVmState::vmSuspended; }
    bool isFree() { return state == tVmState::vmIdle; }

    friend std::ostream &operator<<(std::ostream &os, const VmControlBlock &obj);
};

#endif /* SIMCAN_EX_CONTROL_ENTRIES_H__ */