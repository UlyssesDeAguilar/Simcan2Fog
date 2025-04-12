#include "s2f/architecture/net/switch/Switch.h"

using namespace omnetpp;

Define_Module(Switch);

void Switch::initialize()
{
}

void Switch::finish()
{
    commutationTable.clear();
}

void Switch::handleMessage(cMessage *msg)
{
    auto packet = check_and_cast<L2Protocol *>(msg);

    if (packet->getDestination() == -1)
    {
        cGate *gate = packet->getArrivalGate();
        EV_INFO << "Registering announcement for L2Address: " << packet->getOrigin()
                << " at gate: " << gate->getId() << "\n";

        commutationTable[packet->getOrigin()] = gate->getOtherHalf()->getId();
        delete packet;
    }
    else
    {
        int gateId = commutationTable.at(packet->getDestination());

        // Log details of commutation!
        EV_DEBUG << " Sending payload of type: " << packet->getPayload()->getClassName()
                 << " to: " << packet->getDestination()
                 << " from: " << packet->getOrigin() << "\n";
        send(packet, gate(gateId));
    }
}