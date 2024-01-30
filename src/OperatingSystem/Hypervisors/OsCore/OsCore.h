#ifndef SIMCAN_EX_OS_CORE
#define SIMCAN_EX_OS_CORE

#include "OperatingSystem/Hypervisors/common.h"

namespace hypervisor
{
    // Forward declaration
    class EdgeHypervisor;

    class OsCore
    {
    private:
        DataManager *dataManager;      // The global data manager
        EdgeHypervisor *hypervisor;    // The hypervisor module
        ApplicationBuilder appBuilder; // The application builder (check maybe allowing inheritance?)

    public:
        OsCore() { this->hypervisor = nullptr; }
        void setHypervisor(EdgeHypervisor *hypervisor) { this->hypervisor = hypervisor; }
        void setManager(DataManager *dataManager) { this->dataManager = dataManager; }
        void processSyscall(SM_Syscall *sm);

        void launchApps(SM_UserAPP *request);
        void handleAppTermination(AppControlBlock &app, bool force);
        void handleIOFinish(AppControlBlock &app);
        // void handleSendRequest(AppControlBlock &app, bool completed);
        // void handleBindAndListen(AppControlBlock &app);
    };
};

#endif /* SIMCAN_EX_OS_CORE */