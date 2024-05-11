#include "PingStackDriver.h"

using namespace omnetpp;
using namespace networkio;
using namespace inet;

Define_Module(PingStackDriver);

void PingStackDriver::initialize()
{
    mt = new Event();
    server = par("server");

    // Information about the process
    mt->setPid(UINT32_MAX);
    mt->setIp(0);
    mt->setVmId(0);

    if (!server)
    {
        serverAddress = getModuleByPath("<root>")->par("serverAddress");
        numPings = par("numPings");
        package = new SM_REST_API();
        package->setUrl("service");
        mt->setPackage(package);
        scheduleAt(simTime() + 1.0, mt->dup());
    }
    else
    {
        scheduleAt(simTime() + 0, mt->dup());
    }
}

void PingStackDriver::finish()
{
}

PingStackDriver::~PingStackDriver()
{
    delete mt;
    if (package)
        delete package;
}

void PingStackDriver::handleEvent(cMessage *msg)
{
    mt->setPid(0);
    mt->setCommand(networkio::SOCKET_OPEN);

    if (server)
    {
        EV << "(Server) Init socket\n";
        mt->setKind(StackServiceType::HTTP_PROXY);
        mt->setServiceName("service");
    }
    else
    {
        EV << "(Client) Init socket\n";
        mt->setKind(StackServiceType::HTTP_CLIENT);
        mt->setRips(Ipv4Address(serverAddress).getInt());
    }

    send(mt->dup(), "comm$o");
    mt->setCommand(networkio::SOCKET_SEND);

    delete msg;
}

void PingStackDriver::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage() && mt->getPid() == UINT32_MAX)
    {
        handleEvent(msg);
        return;
    }

    auto event = check_and_cast<Event *>(msg);

    if (server)
    {
        switch (event->getType())
        {
        case networkio::SOCKET_DATA_ARRIVED:
        {
            mt->setRips(event->getRips());
            auto packet = event->getPackageForUpdate();
            EV << "(Server) Recieved data with msg name:" << packet->getName() << "\n";
            mt->setCommand(networkio::SOCKET_SEND);
            mt->setPackage(packet);
            EV << "(Server) Sending ACK";
            send(mt->dup(), "comm$o");
            break;
        }
        case networkio::SOCKET_PEER_CLOSED:
            EV << "(Server) Client closed connection\n";
            EV << "(Server) Shutting down\n";
            break;
        default:
            EV << "(Server) Recieved network I/O event" << event->getType() << "\n";
            break;
        }
    }
    else
    {
        switch (event->getType())
        {
        case networkio::SOCKET_ESTABLISHED:
        {
            EV << "(Client) Connection established, sending first ping\n";
            mt->setCommand(networkio::SOCKET_SEND);
            mt->setRips(event->getRips());
            send(mt->dup(), "comm$o");
            break;
        }
        case networkio::SOCKET_DATA_ARRIVED:
        {
            auto packet = event->getPackageForUpdate();
            EV << "(Client) Recieved data with msg name:" << packet->getName() << "\n";

            if (--numPings > 0)
            {
                EV << "(Client) Preparing next ping\n";
                send(mt->dup(), "comm$o");
            }
            else
            {
                EV << "(Client) All pings done, closing connection\n";
                mt->setCommand(networkio::SOCKET_CLOSE);
                //mt->setPackage(packet);
                send(mt->dup(), "comm$o");
            }
            break;
        }
        default:
            EV << "(Client) Recieved network I/O event" << event->getType() << "\n";
            break;
        }
    }

    delete event;
}