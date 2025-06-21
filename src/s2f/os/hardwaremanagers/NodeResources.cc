#include "s2f/os/hardwaremanagers/NodeResources.h"
using namespace s2f::os;

bool operator==(const NodeResources &node, const VirtualMachine &vm)
{
    return std::make_tuple(vm.getNumCores(), vm.getDiskMiB(), vm.getMemoryMiB()) ==
               std::tie(node.cores, node.diskMiB, node.memoryMiB) &&
           node.hasUserAndVmSpace();
}

bool operator==(const VirtualMachine &vm, const NodeResources &node)
{
    return std::make_tuple(vm.getNumCores(), vm.getDiskMiB(), vm.getMemoryMiB()) ==
               std::tie(node.cores, node.diskMiB, node.memoryMiB) &&
           node.hasUserAndVmSpace();
}

bool operator>=(const NodeResources &node, const VirtualMachine &vm)
{
    return std::tie(node.cores, node.diskMiB, node.memoryMiB) >=
               std::make_tuple(vm.getNumCores(), vm.getDiskMiB(), vm.getMemoryMiB()) &&
           node.hasUserAndVmSpace();
}

bool operator<=(const VirtualMachine &vm, const NodeResources &node)
{
    return std::make_tuple(vm.getNumCores(), vm.getDiskMiB(), vm.getMemoryMiB()) <=
               std::tie(node.cores, node.diskMiB, node.memoryMiB) &&
           node.hasUserAndVmSpace();
}
