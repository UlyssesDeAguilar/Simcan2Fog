#include "UserAppReference.h"

UserAppReference::UserAppReference(Application* appPtr, int numInstances){
    this->appBase = appPtr;
    this->numInstances = numInstances;
}

UserAppReference::~UserAppReference() {
}

Application* UserAppReference::getAppBase(){
    return appBase;
}

int UserAppReference::getNumInstances() const {
    return numInstances;
}

std::string UserAppReference::toString (){

    std::ostringstream info;

        info << appBase->getAppName() << ":" << appBase->getType() << " - " << numInstances << " instances";

    return info.str();
}



