#include "DataEngine.h"

using namespace s2f::data;
using namespace omnetpp;

void DataEngine::initialize() 
{
    // Locate the DataRepository module
    cModule *module = getModuleByPath("^.repository");
    repository = check_and_cast<DataRepository *>(module);
}

void DataEngine::handleMessage(cMessage *msg)
{
    delete msg;
    error("This module doesn't receive messages");
}