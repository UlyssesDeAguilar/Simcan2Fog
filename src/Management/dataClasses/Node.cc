#include "Node.h"

Node::Node(double remainingMemoryGB, double remaininDiskGB, int numCpus, bool racked)
{
    this->remainingMemoryGB = remainingMemoryGB;
    this->remainingDiskGB = remaininDiskGB;
    this->numCpus = numCpus;
    this->racked = racked;
    numAvailableCpus = numCpus;

    // Create the array for the state of each CPU
    cpuState = new tCpuState[numCpus];

    for (int i = 0; i < numCpus; i++)
        cpuState[i] = cpuIdle;
}

// TODO: Al reservar/liberar una CPU, actualizar el número de CPUs disponibles!!!

void Node::setState(tCpuState newState, int cpuIndex)
{

    if ((cpuIndex < 0) || (cpuIndex >= this->numCpus))
    {
        throw omnetpp::cRuntimeError("Index out of bounds while accessing CPU state array. CPU:%d", cpuIndex);
    }
    else
        cpuState[cpuIndex] = newState;
}

tCpuState Node::getState(int cpuIndex)
{

    tCpuState state;

    if ((cpuIndex < 0) || (cpuIndex >= this->numCpus))
    {
        throw omnetpp::cRuntimeError("Index out of bounds while accessing CPU state array. CPU:%d", cpuIndex);
    }
    else
        state = cpuState[cpuIndex];

    return state;
}

std::ostream &operator<<(std::ostream &os, const Node &obj)
{
    os << "Free memory: " << obj.remainingMemoryGB << " GBs  -  ";
    os << "Free disk: " << obj.remainingDiskGB << " GBs  -  ";
    os << "isRacked: " << std::boolalpha << obj.racked << "  -  ";
    os << "CPU states: ";

    for (int i = 0; i < obj.numCpus; i++)
        os << "[" << obj.cpuState[i] << "] ";

    return os;
}
