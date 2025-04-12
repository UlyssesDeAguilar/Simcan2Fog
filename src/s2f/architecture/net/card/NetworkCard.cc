#include "s2f/architecture/net/card/NetworkCard.h"

using namespace omnetpp;
Define_Module(NetworkCard);

void NetworkCard::initialize()
{
    cPar &macValue = par("L2Address");
    macAddress = macValue;

    if (macAddress == -1)
    {
        EV_DEBUG << "MAC address not set. Setting it to " << getId() << "\n";
        macAddress = getId();
        macValue = macAddress;
    }

    if (par("gratuitousBroadcastOnStart"))
    {
        EV_DEBUG << "Sending gratuitous broadcast from L2Address: " << macAddress << "\n";

        auto packet = new L2Protocol();
        packet->setOrigin(macAddress);
        packet->setDestination(-1);
        send(packet, "comm$o");
    }
}

void NetworkCard::finish() {}

void NetworkCard::handleMessage(cMessage *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();
    auto l2message = check_and_cast<L2Protocol *>(msg);
    
    if (l2message->getDestination() == -1)
    {
        EV_INFO << "Received gratuitous broadcast from L2Address: " << l2message->getOrigin() << "\n";
        return;
    }

    if (arrivalGate == gate("comm$i"))
        send(msg, gate("cardOut"));
    else
    {
        l2message->setOrigin(macAddress);
        send(l2message, "comm$o");
    }
}