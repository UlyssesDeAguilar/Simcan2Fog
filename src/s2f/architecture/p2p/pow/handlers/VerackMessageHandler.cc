#include "VerackMessageHandler.h"

inet::Packet *VerackMessageHandler::buildResponse(HandlerContext &ictx)
{
    if (ictx.isClient == false)
        return nullptr;

    auto packet = new inet::Packet("getaddr");
    packet->insertAtBack(buildHeader("getaddr", nullptr));
    return packet;
}
