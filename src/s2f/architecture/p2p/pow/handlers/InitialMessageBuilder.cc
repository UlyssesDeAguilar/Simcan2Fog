#include "InitialMessageBuilder.h"

inet::Packet *InitialMessageBuilder::buildPing(HandlerContext &ictx)
{
    inet::Packet *packet = new inet::Packet("ping");
    auto payload = inet::makeShared<PingPong>();

    payload->setNonce(random());

    packet->insertAtBack(buildHeader("ping", nullptr));
    packet->insertAtBack(payload);
    return packet;
}

inet::Packet *InitialMessageBuilder::buildVersion(HandlerContext &ictx)
{
    auto packet = new inet::Packet("version");
    auto payload = inet::makeShared<Version>();
    auto self = ictx.self;
    auto peer = ictx.peers[ictx.sockFd];

    // Version Message Payload
    payload->setVersion(1);
    payload->setServices(ictx.self.getServices());
    payload->setTimestamp(time(nullptr));

    payload->setAddrRecvServices(peer->getServices());
    payload->setAddrRecvIpAddress(peer->getIpAddress());
    payload->setAddrRecvPort(peer->getPort());

    payload->setAddrTransServices(self.getServices());
    payload->setAddrTransIpAddress(self.getIpAddress());
    payload->setAddrTransPort(self.getPort());

    // TODO: send value back to caller, to append to list of previously used nonces
    payload->setNonce(random());

    // TODO: set these properly once I understand what the fields do
    payload->setUserAgentBytes(5);
    payload->setUserAgent("TODO");
    payload->setStartHeight(0);
    payload->setRelay(true);

    packet->insertAtBack(buildHeader("version", payload));
    packet->insertAtBack(payload);
    return packet;
}
