#include "Application.h"

Application::Application(){
    this->type = "";
    this->appName = "";
}

Application::Application(std::string type, std::string appName){

    // Set the basic parameters
    this->type = type;
    this->appName = appName;
}

Application::~Application() {
    appInstanceParameters.clear();
}

const std::string& Application::getAppName() const {
    return appName;
}

void Application::setAppName(const std::string& appName) {
    this->appName = appName;
}

const std::string& Application::getType() const {
    return type;
}

void Application::setType(const std::string& type) {
    this->type = type;
}

AppParameter* Application::getParameter (int index){

    AppParameter* param;

        // Init...
        param = nullptr;

        if ((index<0) || (index >= appInstanceParameters.size()))
            throw omnetpp::cRuntimeError("Index out of bounds while accessing parameter %d in application %s:%s", index, appName.c_str(), type.c_str());
        else
            param = appInstanceParameters.at(index);

    return param;
}

void Application::insertParameter (AppParameter* param){
    appInstanceParameters.push_back (param);
}

int Application::getNumParameters (){
    return appInstanceParameters.size();
}

AppParameter* Application::getParameterByName(std::string strParameterName)
{
    AppParameter* param;
    int nIndex;
    bool bFound;

    bFound =  false;
    nIndex = 0;

    while(nIndex<appInstanceParameters.size() && !bFound)
    {
        param = appInstanceParameters.at(nIndex);
        if(strParameterName.compare(param->getName())==0)
        {
            bFound=true;
        }
        nIndex++;
    }
    if(!bFound)
    {
        param = nullptr;
    }

    return param;
}
