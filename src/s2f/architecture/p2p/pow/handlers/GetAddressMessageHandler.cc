#include "GetAddressMessageHandler.h"
#include "inet/common/Ptr.h"

inet::Packet *GetAddressMessageHandler::buildResponse(HandlerContext &ictx)
{
    auto packet = new inet::Packet("addr");

    auto payload = inet::makeShared<Address>();

    // Send a copy of active peer info
    for (const auto &iter : ictx.peers)
    {
        PowNetworkPeer *p = new PowNetworkPeer(*iter.second);
        payload->appendIpAddress(p);
    }

    packet->insertAtBack(buildHeader("addr", nullptr));
    packet->insertAtBack(payload);
    return packet;
}
