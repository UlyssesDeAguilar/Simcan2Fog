#include "Echo.h"

Define_Module(Echo);

void Echo::initialize()
{
    ip = par("ip");
    recv = par("recvMode");
    targetIp = par("baseIp");
    addressRange = par("addressRange");
    numAcks = 0;

    if (!recv)
    {
        base = new SIMCAN_Message("SM_Ping");
        scheduleAt(0.0, base);
    }
    cSIMCAN_Core::initialize();
}

void Echo::finish()
{
    if (!recv)
        delete base;
}

cGate *Echo::getOutGate(cMessage *msg)
{
    return gateHalf("comm", cGate::OUTPUT);
}

void Echo::processSelfMessage(cMessage *msg)
{
    for (int i = 0; i < addressRange; i++)
    {
        ServiceURL url(targetIp++);
        auto info = new RoutingInfo();
        info->setUrl(url);
        auto dup = base->dup();
        dup->setControlInfo(info);
        sendRequestMessage(dup, gateHalf("comm", cGate::OUTPUT));
    }
}

void Echo::processRequestMessage(SIMCAN_Message *sm)
{
    if (recv)
    {
        EV << "Recieved message: " << sm->getName() << " at address: " << ip << "\n";
        EV << "Sending ACK" << '\n';
        sm->setIsResponse(true);
        sendResponseMessage(sm);
    }
    else
    {
        EV << "Loopback recieved\n";
        delete sm;
    }
}

void Echo::processResponseMessage(SIMCAN_Message *sm)
{
    if (numAcks < addressRange - 1)
    {
        numAcks++;
    }
    else
    {
        EV << "All acks recieved\n"
           << "Testing loopback\n";

        ServiceURL url(DC_MANAGER_LOCAL_ADDR);
        auto info = new RoutingInfo();
        info->setUrl(url);
        auto dup = base->dup();
        dup->setControlInfo(info);
        sendRequestMessage(dup, gateHalf("comm", cGate::OUTPUT));
    }
    delete sm;
}