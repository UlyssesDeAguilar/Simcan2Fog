#include "../../../management/dataClasses/VirtualMachines/VirtualMachine.h"

VirtualMachine::VirtualMachine(std::string type, double cost, int numCores, double scu, double diskGB, double memoryGB)
{
    this->type = type;
    this->cost = cost;
    this->numCores = numCores;
    this->scu = scu;
    this->diskGB = diskGB;
    this->memoryGB = memoryGB;
}

std::ostream &operator<<(std::ostream &os, const VirtualMachine &obj)
{
    // General information
    return os << "Type:        " << obj.type << "\n"
              << "Cost (h):    " << obj.cost << "\n"
              << "Cores (#):   " << obj.numCores << "\n"
              << "SCU (#):     " << obj.scu << "\n"
              << "Disk (GB):   " << obj.diskGB << "\n"
              << "Memory (GB): " << obj.memoryGB << "\n";
}
