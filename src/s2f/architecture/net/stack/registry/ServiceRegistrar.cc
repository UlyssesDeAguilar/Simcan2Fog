#include "ServiceRegistrar.h"
Define_Module(ServiceRegistrar);

#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/ModuleAccess.h"
#include "s2f/architecture/dns/DnsCommon.h"

using namespace dns;

void ServiceRegistrar::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {
        nameServerIp.tryParse(par("nameServerIp"));
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");

        socket.setOutputGate(gate("socketOut"));
        socket.bind(-1);
        socket.setCallback(this);
        socket.connect(nameServerIp, DNS_PORT);
    }
}

void ServiceRegistrar::handleMessage(cMessage *msg)
{
    socket.processMessage(msg);
}

void ServiceRegistrar::finish()
{
    // Empty stub
}

void ServiceRegistrar::socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent)
{
    EV_WARN << "Data came in (probably a response): Ignoring\n";
    delete packet;
}

void ServiceRegistrar::socketEstablished(TcpSocket *socket)
{
    EV_INFO << "Connection with DNS authoritative nameserver established\n";
}

void ServiceRegistrar::socketPeerClosed(TcpSocket *socket)
{
    EV_WARN << "Connection with DNS authoritative nameserver has been broken\n";
    EV_WARN << "Attempting reconnect ...\n";

    socket->close();
    socket->renewSocket();
}

void ServiceRegistrar::socketFailure(TcpSocket *socket, int code)
{
    EV_WARN << "Connection with DNS authoritative nameserver has failed\n";
    EV_WARN << "Failure reason: ";

    if (code == TCP_I_CONNECTION_RESET)
        EV << "Connection reset\n";
    else if (code == TCP_I_CONNECTION_REFUSED)
        EV << "Connection refused\n";
    /*else if (code == TCP_I_TIMEOUT)
        EV << "Connection timed out\n";*/

    EV_WARN << "Attempting reconnect ...\n";

    socket->close();
    socket->renewSocket();
}

void ServiceRegistrar::registerService(const char *domain)
{
    /*Enter_Method("Registering service %s\n", domain);
    DomainBinding binding;
    ResourceRecord record;

    auto packet = new Packet("Registration request");
    const auto &request = makeShared<DnsRegistrationRequest>();

    record.type = ResourceRecord::RR_Type::A;
    record.domain = domain;
    record.ip = localIp;
    binding.record = record;
    request->insertBinding(binding);

    packet->insertAtBack(request);
    socket.send(packet);*/
}

void ServiceRegistrar::unregisterService(const char *domain)
{
    /*Enter_Method("Unregistering service %s\n", domain);
    DomainBinding binding;
    ResourceRecord record;

    auto packet = new Packet("Registration request");
    const auto &request = makeShared<DnsRegistrationRequest>();

    record.type = ResourceRecord::RR_Type::A;
    record.domain = domain;
    record.ip = localIp;
    binding.record = record;
    binding.unregisterIfPresent = true;
    
    request->insertBinding(binding);

    packet->insertAtBack(request);
    socket.send(packet);*/
}