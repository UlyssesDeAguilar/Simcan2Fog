#include "PowMessageBuilder.h"
#include "PowMsgAddress_m.h"
#include "PowMsgHeader_m.h"
#include "PowMsgVersion_m.h"
#include "inet/networklayer/common/L3Address.h"

using namespace s2f::p2p;
using namespace inet;

Ptr<PowMsgHeader> PowMessageBuilder::buildMessageHeader(const char *commandName, Ptr<FieldsChunk> payload)
{
    auto header = makeShared<PowMsgHeader>();

    // Message Header
    header->setCommandName(commandName);
    header->setStartString(StartString::MAIN_NET);
    header->setPayloadSize(0);
    header->setChecksum("TODO");

    return header;
}

Ptr<PowMsgVersion> PowMessageBuilder::buildVersionMessage()
{
    auto payload = makeShared<PowMsgVersion>();

    // Version Message Payload
    payload->setVersion(1);
    payload->setServices(PowServiceType::NODE_NETWORK);
    payload->setTimestamp(time(nullptr));

    payload->setAddrRecvServices(payload->getServices());
    payload->setAddrRecvIpAddress("TODO");
    payload->setAddrRecvPort(8333);

    payload->setAddrTransServices(payload->getServices());
    payload->setAddrTransIpAddress("TODO");
    payload->setAddrTransPort(8333);

    payload->setNonce(0);
    payload->setUserAgentBytes(5);
    payload->setUserAgent("TODO");

    payload->setStartHeight(0);
    payload->setRelay(true);

    return payload;
}

Ptr<PowMsgAddress> PowMessageBuilder::buildAddrMessage()
{
    auto payload = makeShared<PowMsgAddress>();
    PowNetworkPeer *peer = new PowNetworkPeer;

    peer->setPort(8333);
    peer->setTime(0);
    peer->setIpAddress(L3Address("10.0.0.5"));
    peer->setServices(PowServiceType::NODE_NETWORK);

    payload->appendIpAddress(peer);

    return payload;
}
