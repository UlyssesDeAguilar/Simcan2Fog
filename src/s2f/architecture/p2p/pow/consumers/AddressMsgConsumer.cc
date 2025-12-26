#include "AddressMsgConsumer.h"

using namespace s2f::p2p::pow;

int findIpInPeers(std::map<int, PowPeer *> &peers, inet::L3Address ip)
{
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto &p)
                           { return p.second->getIpAddress() == ip; });
    return it != peers.end() ? it->first : 0;
}

IPowMsgResponse AddressMsgConsumer::handleMessage(IPowMsgContext &ictx)
{
    IPowMsgResponse response{};
    auto payload = ictx.msg->peekData<Address>();

    response.action = OPEN;
    for (int i = 0; i < payload->getIpAddressArraySize(); i++)
    {
        auto p = const_cast<PowPeer *>(payload->getIpAddress(i));

        if (p->getIpAddress() == ictx.self.getIpAddress() || findIpInPeers(ictx.peers, p->getIpAddress()))
            delete p;
        else
            response.peers.push_back(p);
    }

    return response;
}
