#include "EdgeHypervisor.h"
using namespace hypervisor;
Define_Module(EdgeHypervisor);

void EdgeHypervisor::initialize(int stage)
{
    switch (stage)
    {
    case LOCAL:
        appsVector = getModuleByPath("^.apps");
        break;
    case NEAR:
        // Initialize the free PID table
    default:
        break;
    }

    Hypervisor::initialize(stage);
}