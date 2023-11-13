#include "RequestMultiplexer.h"
Define_Module(RequestMultiplexer);

void RequestMultiplexer::initialize()
{
    // Get if debug information should be printed
    debug = getParentModule()->par("debug");

    // Get gate info
    outGate = gate("gOutput");
    gInputId = gate("gInput")->getId();

    // Maybe we can avoid the wiring by assigning statically the modules !
    cModule *ma = getModuleByPath("^.app[0]");
    cModule *res = getModuleByPath("^.app[1]");

    ma->gate("moduleOut")->connectTo(gate("MessageOut"));
    res->gate("moduleOut")->connectTo(gate("ResolverOut"));

    gate("MessageIn")->connectTo(ma->gate("moduleIn"));
    gate("ResolverIn")->connectTo(res->gate("moduleIn"));
}

void RequestMultiplexer::handleMessage(cMessage *msg)
{
    auto arrivalId = msg->getArrivalGate()->getId();

    // Distinguish between request - response
    if (arrivalId != gInputId)
    {
        if (debug)
            EV << "Recieved response, sending back to module" << endl;
        send(msg, outGate);
    }
    else
    {
        auto resolverRequest = dynamic_cast<SM_ResolverRequest *>(msg);

        if (debug)
            EV << "Recieved request: " << msg->getClassName() << " starting to process ..." << endl;
        
        // Mutliplex address resolution and general message sending
        if (resolverRequest != nullptr)
            send(msg, gate("ResolverIn"));
        else
            send(msg, gate("MessageIn"));
    }
}

void RequestMultiplexer::finish()
{
    // Empty as it doesn't allocate any new resources
}
