#include "AddressMessageHandler.h"

int findIpInPeers(std::map<int, PowNetworkPeer *> &peers, inet::L3Address ip)
{
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto &p)
                           { return p.second->getIpAddress() == ip; });
    return it != peers.end() ? it->first : 0;
}

HandlerResponse AddressMessageHandler::handleMessage(inet::Packet *msg, HandlerContext &ictx)
{
    HandlerResponse response{};
    auto payload = msg->peekData<Address>();

    response.action = OPEN;
    for (int i = 0; i < payload->getIpAddressArraySize(); i++)
    {
        auto p = const_cast<PowNetworkPeer *>(payload->getIpAddress(i));

        if (p->getIpAddress() == ictx.localIp || findIpInPeers(ictx.peers, p->getIpAddress()))
            delete p;
        else
        {
            p->setState(ConnectionState::CANDIDATE);
            response.peers.push_back(p);
        }
    }

    return response;
}
