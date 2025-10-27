#include "PowMessageBuilder.h"
#include "messages/Address_m.h"
#include "messages/Header_m.h"
#include "messages/Version_m.h"
#include <cstdint>

using namespace s2f::p2p;
using namespace inet;

Ptr<Header> PowMessageBuilder::buildMessageHeader(const char *commandName, Ptr<FieldsChunk> payload)
{
    auto header = makeShared<Header>();

    // Message Header
    header->setCommandName(commandName);
    header->setStartString(pow::MAIN_NET);

    // TODO: determine size from payload and sha256 to create a checksum
    header->setPayloadSize(0);
    header->setChecksum("TODO");

    return header;
}

Ptr<Version> PowMessageBuilder::buildVersionMessage(int32_t version, PowNetworkPeer &self, PowNetworkPeer &peer)
{
    auto payload = makeShared<Version>();

    // Version Message Payload
    payload->setVersion(version);
    payload->setServices(self.getServices());
    payload->setTimestamp(time(nullptr));

    payload->setAddrRecvServices(peer.getServices());
    payload->setAddrRecvIpAddress(peer.getIpAddress());
    payload->setAddrRecvPort(peer.getPort());

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

    return payload;
}

Ptr<Address> PowMessageBuilder::buildAddrMessage(std::map<int, PowNetworkPeer *> &peers)
{
    auto payload = makeShared<Address>();

    // Send a copy of active peer info
    for (const auto &iter : peers)
    {
        PowNetworkPeer *p = new PowNetworkPeer(*iter.second);
        payload->appendIpAddress(p);
    }

    return payload;
}
