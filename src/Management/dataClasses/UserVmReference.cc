#include "UserVmReference.h"

UserVmReference::UserVmReference(VirtualMachine* vmPtr, int numInstances, int nRentTime){
    this->vmBase = vmPtr;
    this->numInstances = numInstances;
    this->nRentTime = nRentTime;
}

UserVmReference::~UserVmReference() {
}

VirtualMachine* UserVmReference::getVmBase(){
    return vmBase;
}

int UserVmReference::getNumInstances() const {
    return numInstances;
}
int UserVmReference::getRentTime() const {
    return nRentTime;
}

std::string UserVmReference::toString (){

    std::ostringstream info;

    info << vmBase->getType() << " - " << numInstances << " instances " << " - " << nRentTime << " renting hours";

    return info.str();
}



