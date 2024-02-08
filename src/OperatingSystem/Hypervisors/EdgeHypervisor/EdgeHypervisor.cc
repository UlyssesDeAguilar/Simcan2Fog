#include "EdgeHypervisor.h"
using namespace hypervisor;
Define_Module(EdgeHypervisor);

void EdgeHypervisor::initialize(int stage)
{
    switch (stage)
    {
    case INNER_STAGE:
        appsVector = getModuleByPath("^.apps");
        break;
    case LOCAL_STAGE:
        // Initialize the free PID table
    default:
        break;
    }
}