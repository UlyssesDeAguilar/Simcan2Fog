#include "RequestMultiplexer.h"
Define_Module(RequestMultiplexer);

void RequestMultiplexer::initialize()
{
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
        send(msg, outGate);
    else
    {
        auto resolverRequest = dynamic_cast<SM_ResolverRequest *>(msg);

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
