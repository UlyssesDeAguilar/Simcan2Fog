#include "Node.h"

Node::Node(double remainingMemoryGB, double remaininDiskGB, int numCpus, bool racked){

    int i;

        this->remainingMemoryGB = remainingMemoryGB;
        this->remaininDiskGB = remaininDiskGB;
        this->numCpus = numCpus;
        this->racked = racked;
        numAvailableCpus = numCpus;

        // Create the array for the state of each CPU
        cpuState = new tCpuState[numCpus];

        for (i=0; i<numCpus; i++)
            cpuState[i] = cpuIdle;
}

//TODO: Al reservar/liberar una CPU, actualizar el número de CPUs disponibles!!!


void Node::setState (tCpuState newState, int cpuIndex){

    if ((cpuIndex<0) || (cpuIndex >= this->numCpus)){
        throw omnetpp::cRuntimeError("Index out of bounds while accessing CPU state array. CPU:%d", cpuIndex);
    }
    else
        cpuState[cpuIndex] = newState;
}

tCpuState Node::getState (int cpuIndex){

    tCpuState state;

        if ((cpuIndex<0) || (cpuIndex >= this->numCpus)){
            throw omnetpp::cRuntimeError("Index out of bounds while accessing CPU state array. CPU:%d", cpuIndex);
        }
        else
            state = cpuState[cpuIndex];

    return state;
}

int Node::getNumCpus() const {
    return numCpus;
}

void Node::setNumCpus(int numCpus) {
    this->numCpus = numCpus;
}

int Node::getNumAvailableCpus() const {
    return numAvailableCpus;
}

void Node::setNumAvailableCpus(int numAvailableCpus) {
    this->numAvailableCpus = numAvailableCpus;
}

bool Node::isRacked() const {
    return racked;
}

void Node::setRacked(bool racked) {
    this->racked = racked;
}

double Node::getRemaininDiskGb() const {
    return remaininDiskGB;
}

void Node::setRemaininDiskGb(double remaininDiskGb) {
    remaininDiskGB = remaininDiskGb;
}

double Node::getRemainingMemoryGb() const {
    return remainingMemoryGB;
}

void Node::setRemainingMemoryGb(double remainingMemoryGb) {
    remainingMemoryGB = remainingMemoryGb;
}

const std::string& Node::getIp() const {
    return ip;
}

void Node::setIp(const std::string& ip) {
    this->ip = ip;
}
std::string Node::toString (){

    std::ostringstream text;
    int i;

        text << "Free memory: " << remainingMemoryGB << " GBs  -  ";
        text << "Free disk: "  << remaininDiskGB << " GBs  -  ";
        text << "isRacked: " << std::boolalpha << racked << "  -  ";
        text << "CPU states: ";

        for (i=0; i< this->numCpus; i++)
            text << "[" << cpuState[i] << "] ";

    return text.str();
}


