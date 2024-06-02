#ifndef SIMCAN_EX_OS_CORE
#define SIMCAN_EX_OS_CORE

#include "OperatingSystem/Hypervisors/common.h"
#include "Architecture/Network/Stack/NetworkIOEvent_m.h"

namespace hypervisor
{
    // Forward declaration
    class Hypervisor;

    class OsCore
    {
    private:
        DataManager *dataManager;         // The global data manager
        HardwareManager *hardwareManager; // The hardware manager of the node
        Hypervisor *hypervisor;           // The hypervisor module
        ApplicationBuilder appBuilder;    // The application builder (check maybe allowing inheritance?)

    public:
        typedef std::vector<AppRequest>::iterator app_iterator;

        OsCore() { this->hypervisor = nullptr; }
        void setUp(Hypervisor *h, DataManager *dm, HardwareManager *hm);
        void processSyscall(Syscall *sm);

        void launchApps(SM_UserAPP *request, uint32_t vmId, app_iterator begin, app_iterator end, const std::string &globalVmId);
        void handleAppTermination(AppControlBlock &app, tApplicationState exitStatus);
        void fillNetworkData(Syscall *sys, networkio::CommandEvent *e);
        // void handleSendRequest(AppControlBlock &app, bool completed);
        // void handleBindAndListen(AppControlBlock &app);
    };
};

#endif /* SIMCAN_EX_OS_CORE */
