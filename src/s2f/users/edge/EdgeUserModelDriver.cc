#include "../../users/edge/EdgeUserModelDriver.h"

Define_Module(EdgeUserModelDriver);

using namespace inet;

void EdgeUserModelDriver::initialize(int stage)
{
    if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        // Retrieve the local address from the network layer
        cModule *host = getParentModule();
        address = L3AddressResolver().addressOf(host);
    }
    ApplicationBase::initialize(stage);
}

void EdgeUserModelDriver::handleStartOperation(LifecycleOperation *operation)
{
    // Configure the local socket
    localNet.setOutputGate(gate("socketOut"));
    localNet.setCallback(this);
}

void EdgeUserModelDriver::handleStopOperation(LifecycleOperation *operation)
{
    error("MessageAdapter: Stopping not supported");
}

void EdgeUserModelDriver::handleCrashOperation(LifecycleOperation *operation)
{
    error("MessageAdapter: Crashing not supported");
}

void EdgeUserModelDriver::finish()
{
    // Close the server socket
    localNet.close();

    // Release the sockets
    globalNet.deleteSockets();
}