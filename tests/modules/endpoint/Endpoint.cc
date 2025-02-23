#include "Endpoint.h"
#include <memory>

Define_Module(Endpoint);

void Endpoint::initialize()
{
    nameToResolve = par("nameToResolve");
    auto startEvent = new cMessage();
    scheduleAt(simTime() + 0.0, startEvent);
}
void Endpoint::finish()
{
    // Currently empty
}

void Endpoint::handleMessage(cMessage *msg)
{
    /*if (msg->isSelfMessage())
    {
        auto request = new SM_ResolverRequest();
        request->setDomainName(nameToResolve);
        EV << "Sending request for: " << nameToResolve << endl;
        send(request, gate("endpointOut"));
    }else{
        auto recvMsg = dynamic_cast<SM_ResolverRequest*>(msg);
        if (recvMsg == nullptr)
        {
            EV << "Unexpected message" << endl;
        }else
        {
            if (recvMsg->getResult() == SC_OK)
                EV << "Resolved IP: " << recvMsg->getResolvedIp() << endl;
            else
                EV << "No address was resolved" << endl;
        }
    }*/

    delete msg;
}