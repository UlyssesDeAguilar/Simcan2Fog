#include "NodeInfo.h"

NodeInfo::NodeInfo(std::string name, bool storage, double totalDiskGB, double totalMemoryGB, int numCpus, int cpuSpeed){
    this->name = "";
    this->storage = storage;
    this->totalDiskGB = totalDiskGB;
    this->totalMemoryGB = totalMemoryGB;
    this->numCPUs = numCpus;
    this->cpuSpeed = cpuSpeed;
}

int NodeInfo::getCpuSpeed() const {
    return cpuSpeed;
}

void NodeInfo::setCpuSpeed(int cpuSpeed) {
    this->cpuSpeed = cpuSpeed;
}

const std::string& NodeInfo::getName() const {
    return name;
}

void NodeInfo::setName(const std::string& name) {
    this->name = name;
}

int NodeInfo::getNumCpUs() const {
    return numCPUs;
}

void NodeInfo::setNumCpUs(int numCpUs) {
    numCPUs = numCpUs;
}

bool NodeInfo::isStorage() const {
    return storage;
}

void NodeInfo::setStorage(bool storage) {
    this->storage = storage;
}

double NodeInfo::getTotalDiskGb() const {
    return totalDiskGB;
}

void NodeInfo::setTotalDiskGb(double totalDiskGb) {
    totalDiskGB = totalDiskGb;
}

double NodeInfo::getTotalMemoryGb() const {
    return totalMemoryGB;
}

void NodeInfo::setTotalMemoryGb(double totalMemoryGb) {
    totalMemoryGB = totalMemoryGb;
}

std::string NodeInfo::toString (){

    std::ostringstream info;
    std::string type;

        if (storage)
            type = "Storage";
        else
            type = "Computing";

        info << name << " "     << type             << " ";
        info << "Memory: "      << totalMemoryGB    << " GBs  -  ";
        info << "Disk: "        << totalDiskGB      << " GBs  -  ";
        info << "CPU speed: "   << this->cpuSpeed   << " MPIS -  #" << numCPUs << " cores";

    return info.str();
}
