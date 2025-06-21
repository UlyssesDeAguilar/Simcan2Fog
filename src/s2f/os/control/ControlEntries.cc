#include "ControlEntries.h"
using namespace s2f::os;

VmControlBlock::~VmControlBlock() { free(); }

void VmControlBlock::initialize(int vmId, const VirtualMachine *vmType, const char *userId, size_t apps)
{
    this->vmId = vmId;
    this->vmType = vmType;
    this->userId = userId;
    state = tVmState::vmIdle;
    request = nullptr;
    this->apps.resize(apps);
}

void VmControlBlock::free()
{
    state = tVmState::vmIdle;
    vmType = nullptr;
    callBuffer.clear();
    apps.clear();
    if (request)
        delete request;
}

void VmControlBlock::resize(size_t apps)
{
    if (apps > this->apps.size())
        this->apps.resize(apps);
}

std::ostream &operator<<(std::ostream &os, const VmControlBlock &obj)
{
    return os << "VmId:     " << obj.vmId << '\n';
}
