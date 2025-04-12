#include "Echo.h"
#include "s2f/architecture/net/protocol/L2Protocol_m.h"
using namespace switchtest;
Define_Module(Echo);

void Echo::initialize()
{
    ip = par("ip");
    recv = par("recvMode");
    targetIp = par("baseIp");
    addressRange = par("addressRange");
    numAcks = 0;

    if (!recv)
        scheduleAt(0.0, new cMessage("Start test"));
}

void Echo::finish()
{
    if (!recv)
    {
        EV_INFO << "Number of ack's received: " << numAcks << "\n";
    }
}

void Echo::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
	{
		delete msg;
		auto pingMessage = new cMessage("Ping request");
		auto packet = L2Protocol::encapsulate(pingMessage, ip, targetIp);

		for (int i = 0; i < addressRange; i++)
		{
			packet->setDestination(targetIp + i);
			send(packet->dup(), "networkOut");
		}
		delete packet;
		return;
	}

	if (recv)
	{
		auto packet = check_and_cast<L2Protocol *>(msg);
		EV_INFO << "Host: " << getFullName() << " received ping from: " << packet->getOrigin() << "\n";
		auto pingMessage = new cMessage("Ping reply");
		auto reply = L2Protocol::encapsulate(pingMessage, ip, packet->getOrigin());
		send(reply, "networkOut");
		delete packet;
	}
	else
	{
		auto packet = check_and_cast<L2Protocol *>(msg);
		numAcks++;
		EV_INFO << "Received reply from host: " << packet->getOrigin() << "\n";
		delete packet;
	}
}
