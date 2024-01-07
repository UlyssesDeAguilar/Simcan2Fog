#include "CloudUser.h"

CloudUser::~CloudUser() {
    virtualMachines.clear();
}

void CloudUser::insertVirtualMachine(VirtualMachine *vmPtr, int numInstances, int nRentTime) {
    auto newElement = new UserVmReference(vmPtr, numInstances, nRentTime);
    virtualMachines.push_back(newElement);
}

std::string CloudUser::toString() const{
    std::ostringstream info;
    int i;

    // info << "Type:" << type << " - Priority: " << tPriorityTypeLabel[priorityType] << " - Sla: " << sla->getType() << " -  #Instances:" << numInstances << std::endl;
    info << "Type:" << type << " -  #Instances:" << numInstances << "\n";

    // Prints applications
    i = 0;
    for (const auto& app: applications)
       info << "\t  + App[" << i++ << "] -> " << app->toString() << "\n";

    // Prints virtual machines
    i = 0;
    for (const auto& vm: virtualMachines)
       info << "\t  + VM[" << i++ << "] -> " << vm->toString() << "\n";

    return info.str();
}
