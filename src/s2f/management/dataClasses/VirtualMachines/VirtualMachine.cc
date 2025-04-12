#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"


std::ostream &operator<<(std::ostream &os, const VirtualMachine &obj)
{
    // General information
    return os << "Type:        " << obj.type << "\n"
              << "Cost (h):    " << obj.cost << "\n"
              << "Cores (#):   " << obj.numCores << "\n"
              << "SCU (#):     " << obj.scu << "\n"
              << "Disk (GB):   " << obj.diskMiB / 1024.0 << "\n"
              << "Memory (GB): " << obj.memoryMiB / 1024.0 << "\n";
}
