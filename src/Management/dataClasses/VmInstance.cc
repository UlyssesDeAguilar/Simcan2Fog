#include "VmInstance.h"

VmInstance::VmInstance(std::string vmType, int currentInstanceIndex, int totalVmInstances, std::string userID){

    std::ostringstream osStream;

    // Generate the unique name -> vmType-userID[currentInstanceIndex/totalVmInstances]
    osStream << vmType << "-" << userID << "-[" << currentInstanceIndex+1 << "/" << totalVmInstances << "]";
    this->vmInstanceID = osStream.str();

    // Vm State
    this->vmType = vmType;
    this->instanceNumber = currentInstanceIndex;
    this->userID = userID;
    this->state = vmIdle;
}

VmInstance::~VmInstance() {
}

int VmInstance::getInstanceNumber() const {
    return instanceNumber;
}

tVmState VmInstance::getState() const {
    return state;
}

void VmInstance::setState(tVmState state) {
    this->state = state;
}

const std::string& VmInstance::getUserId() const {
    return userID;
}

const std::string& VmInstance::getVmInstanceId() const {
    return vmInstanceID;
}

const std::string& VmInstance::getVmType() const {
    return vmType;
}

std::string VmInstance::toString (){

    std::ostringstream info;

            info << vmInstanceID << "  - State: " << stateToString(state);

        return info.str();
}

std::string VmInstance::stateToString (tVmState vmState){

    std::string result;

        switch (vmState){

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

