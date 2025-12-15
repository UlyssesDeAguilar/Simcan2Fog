#include "GetAddressMsgConsumer.h"

using namespace s2f::p2p::pow;

inet::Packet *GetAddressMsgConsumer::buildResponse(IPowMsgContext &ictx)
{
    auto packet = new inet::Packet("addr");

    auto payload = inet::makeShared<Address>();

    // Send a copy of active peer info
    for (const auto &iter : ictx.peers)
    {
        PowPeer *p = new PowPeer(*iter.second);
        payload->appendIpAddress(p);
    }

    packet->insertAtBack(buildHeader("addr", nullptr));
    packet->insertAtBack(payload);
    return packet;
}
