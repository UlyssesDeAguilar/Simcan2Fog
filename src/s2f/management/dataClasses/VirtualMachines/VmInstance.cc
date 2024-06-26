#include "../../../management/dataClasses/VirtualMachines/VmInstance.h"

VmInstance::VmInstance(const VmInstanceType *type, int currentInstanceIndex, int totalVmInstances)
{

    std::ostringstream osStream;

    // Generate the unique name -> vmType-userID[currentInstanceIndex/totalVmInstances]
    osStream << type->base->getType() << "-" << type->userId << "-[" << currentInstanceIndex + 1 << "/" << totalVmInstances << "]";
    this->id = osStream.str();

    // Vm State
    this->type = type;
    this->instanceNumber = currentInstanceIndex;
    this->state = vmIdle;
}

std::ostream & operator<<(std::ostream &os, const VmInstance &obj)
{
    return os << obj.id << "  - State: " << obj.stateToString(obj.state);
}

std::string VmInstance::stateToString(tVmState vmState) const
{

    std::string result;

    switch (vmState)
    {

    case vmIdle:
        result = "idle";
        break;
    case vmAccepted:
        result = "accepted";
        break;
    case vmRunning:
        result = "running";
        break;
    case vmFinished:
        result = "finished";
        break;
    default:
        result = "unknown state";
        break;
    }

    return result;
}
