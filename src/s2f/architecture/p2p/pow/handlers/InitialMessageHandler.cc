#include "InitialMessageHandler.h"

inet::Packet *InitialMessageHandler::buildResponse(HandlerContext &ictx)
{
    auto packet = new inet::Packet("version");
    auto payload = inet::makeShared<Version>();
    auto self = ictx.self;
    auto peer = ictx.peers[ictx.sockFd];

    // TODO: receive version from context
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
