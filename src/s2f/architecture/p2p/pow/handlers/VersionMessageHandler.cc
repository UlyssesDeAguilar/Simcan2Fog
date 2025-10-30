#include "VersionMessageHandler.h"
#include "s2f/architecture/p2p/pow/handlers/IMessageHandler.h"

HandlerResponse VersionMessageHandler::handleMessage(struct HandlerContext &ictx)
{
    auto payload = ictx.msg->peekData<Version>();
    auto p = ictx.peers[ictx.sockFd];

    // TODO: actions->ERASE to close socket and disconnect peer
    // TODO: better nonce+version verification
    if (payload->getVersion() != 1)
        return {.action = NOACTION};

    p->setServices(payload->getAddrTransServices());
    p->setTime(payload->getTimestamp());
    p->setPort(payload->getAddrTransPort());

    return {.action = NOACTION};
}

inet::Packet *VersionMessageHandler::buildResponse(HandlerContext &ictx)
{
    auto packet = new inet::Packet("verack");
    packet->insertAtBack(buildHeader("verack", nullptr));
    return packet;
}
