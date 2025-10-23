#include "PowMessageBuilder.h"
#include "PowMsgAddress_m.h"
#include "PowMsgHeader_m.h"
#include "PowMsgVersion_m.h"
#include <cstdint>

using namespace s2f::p2p;
using namespace inet;

Ptr<PowMsgHeader> PowMessageBuilder::buildMessageHeader(const char *commandName, Ptr<FieldsChunk> payload)
{
    auto header = makeShared<PowMsgHeader>();

    // Message Header
    header->setCommandName(commandName);
    header->setStartString(pow::MAIN_NET);

    // TODO: determine size from payload and sha256 to create a checksum
    header->setPayloadSize(0);
    header->setChecksum("TODO");

    return header;
}

Ptr<PowMsgVersion> PowMessageBuilder::buildVersionMessage(int32_t version, PowNetworkPeer &self, PowNetworkPeer &peer)
{
    auto payload = makeShared<PowMsgVersion>();

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

Ptr<PowMsgAddress> PowMessageBuilder::buildAddrMessage(std::map<int, PowNetworkPeer *> &peers)
{
    auto payload = makeShared<PowMsgAddress>();

    // Send a copy of active peer info
    for (const auto &iter : peers)
    {
        PowNetworkPeer *p = new PowNetworkPeer(*iter.second);
        payload->appendIpAddress(p);
    }

    return payload;
}
