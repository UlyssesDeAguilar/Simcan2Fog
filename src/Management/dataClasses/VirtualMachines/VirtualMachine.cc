#include "VirtualMachine.h"

VirtualMachine::VirtualMachine(std::string type, double cost, int numCores, double scu, double diskGB, double memoryGB)
{
    this->type = type;
    this->cost = cost;
    this->numCores = numCores;
    this->scu = scu;
    this->diskGB = diskGB;
    this->memoryGB = memoryGB;
}

VirtualMachine::~VirtualMachine()
{
}

std::string VirtualMachine::featuresToString()
{

    std::ostringstream info;

    info << "cost: " << cost << "  -  #Cores:" << numCores << "  -  SCU:" << scu;
    info << "  -  Disk space: " << diskGB << " GB  -  Memory space: " << memoryGB << " GB";

    return info.str();
}

std::ostream &operator<<(std::ostream &os, const VirtualMachine &obj)
{
    // General information
    return os << "Type:        " << obj.type << "\n"
              << "Cost (h):    " << obj.cost << "\n"
              << "Cores (#):   " << obj.numCores << "\n"
              << "Disk (GB):   " << obj.diskGB << "\n"
              << "Memory (GB): " << obj.diskGB << "\n";
}
