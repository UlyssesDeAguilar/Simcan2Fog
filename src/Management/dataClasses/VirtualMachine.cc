#include "VirtualMachine.h"

VirtualMachine::VirtualMachine() {

    this->type = "";
    this->cost = 0.0;
    this->numCores = 0;
    this->scu = 0.0;
    this->diskGB = 0.0;
    this->memoryGB = 0.0;
}

VirtualMachine::VirtualMachine(std::string type, double cost, int numCores, double scu, double diskGB, double memoryGB){

    this->type = type;
    this->cost = cost;
    this->numCores = numCores;
    this->scu = scu;
    this->diskGB = diskGB;
    this->memoryGB = memoryGB;
}

VirtualMachine::~VirtualMachine() {
}

double VirtualMachine::getCost() const {
    return cost;
}

void VirtualMachine::setCost(double cost) {
    this->cost = cost;
}

double VirtualMachine::getDiskGb() const {
    return diskGB;
}

void VirtualMachine::setDiskGb(double diskGb) {
    diskGB = diskGb;
}

double VirtualMachine::getMemoryGb() const {
    return memoryGB;
}

void VirtualMachine::setMemoryGb(double memoryGb) {
    memoryGB = memoryGb;
}

int VirtualMachine::getNumCores() const {
    return numCores;
}

void VirtualMachine::setNumCores(int numCores) {
    this->numCores = numCores;
}

double VirtualMachine::getScu() const {
    return scu;
}

void VirtualMachine::setScu(double scu) {
    this->scu = scu;
}

const std::string& VirtualMachine::getType() const {
    return type;
}

void VirtualMachine::setType(const std::string& type) {
    this->type = type;
}

std::string VirtualMachine::featuresToString (){

    std::ostringstream info;

        info << "cost: " << cost << "  -  #Cores:" << numCores << "  -  SCU:" << scu;
        info << "  -  Disk space: " << diskGB << " GB  -  Memory space: " << memoryGB << " GB";

    return info.str();
}
