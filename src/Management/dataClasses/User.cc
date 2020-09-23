#include "User.h"

User::User(std::string type, int numInstances){
    this->type = type;
    this->numInstances = numInstances;
}

User::~User() {
    applications.clear();
}

void User::insertApplication(Application* appPtr, int numInstances){

    UserAppReference* newElement;

        newElement = new UserAppReference(appPtr, numInstances);
        applications.push_back(newElement);
}

UserAppReference* User::getApplication (int index){

    UserAppReference* element;

        element = nullptr;

        if ((index<0) || (index>=applications.size())){
            throw omnetpp::cRuntimeError("Index out of bounds while accessing application (Element) %d in User:%s", index, type.c_str());
        }
        else
            element = applications.at(index);

    return element;
}

int User::getNumApplications(){
    return applications.size();
}

const std::string& User::getType() const {
    return type;
}

int User::getNumInstances() const {
    return numInstances;
}

std::string User::toString (){

    std::ostringstream info;
    int i;

        info << "Type:" << type << "  -  #Instances:" << numInstances << std::endl;

            // Parses applications
            for (i=0; i<applications.size(); i++){
                info << "\tApp[" << i << "] -> " << applications.at(i)->toString();
            }

    return info.str();
}


