#include "CloudUser.h"

CloudUser::CloudUser(std::string type, int numInstances) : User(type, numInstances){

}

CloudUser::~CloudUser() {
    virtualMachines.clear();
}

void CloudUser::insertVirtualMachine(VirtualMachine *vmPtr, int numInstances, int nRentTime) {
    UserVmReference *newElement;

    newElement = new UserVmReference(vmPtr, numInstances, nRentTime);
    virtualMachines.push_back(newElement);
}

UserVmReference* CloudUser::getVirtualMachine (int index) {
    UserVmReference* element = nullptr;

    if ((index<0) || (index>=virtualMachines.size()))
        throw omnetpp::cRuntimeError("Index out of bounds while accessing application (Element) %d in User:%s", index, type.c_str());
    else
        element = virtualMachines.at(index);

    return element;
}

int CloudUser::getNumVirtualMachines() {
    return virtualMachines.size();
}

std::string CloudUser::toString() {
    std::ostringstream info;

//        info << "Type:" << type << " - Priority: " << tPriorityTypeLabel[priorityType] << " - Sla: " << sla->getType() << " -  #Instances:" << numInstances << std::endl;
    info << "Type:" << type << " -  #Instances:" << numInstances << std::endl;

    // Parses applications
    for (int i = 0; i < applications.size(); i++)
       info << "\t  + App[" << i << "] -> " << applications.at(i)->toString() << std::endl;

    // Parses VMs
    for (int i = 0; i < virtualMachines.size(); i++)
       info << "\t  + VM[" << i << "] -> " << virtualMachines.at(i)->toString() << std::endl;

    return info.str();
}
