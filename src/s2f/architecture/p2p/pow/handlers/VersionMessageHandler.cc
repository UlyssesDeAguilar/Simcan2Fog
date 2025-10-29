#include "VersionMessageHandler.h"
#include "s2f/architecture/p2p/pow/handlers/IMessageHandler.h"

struct HandlerResponse VersionMessageHandler::handleMessage(inet::Packet *msg, struct HandlerContext &ictx)
{
    auto payload = msg->peekData<Version>();
    auto p = ictx.peers[ictx.sockFd];

    // TODO: actions->ERASE to close socket and disconnect peer
    // TODO: better nonce+version verification
    if (payload->getVersion() != 1)
        return {NOACTION};

    p->setServices(payload->getAddrTransServices());
    p->setTime(payload->getTimestamp());
    p->setPort(payload->getAddrTransPort());

    return {NOACTION};
}

inet::Packet *VersionMessageHandler::buildResponse(HandlerContext &ictx)
{
    auto packet = new inet::Packet("verack");
    packet->insertAtBack(buildHeader("verack", nullptr));
    return packet;
}
