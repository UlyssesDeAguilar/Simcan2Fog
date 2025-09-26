#include "PowMessageBuilder.h"
#include "PowMsgAddress_m.h"
#include "PowMsgHeader_m.h"
#include "PowMsgVersion_m.h"
#include "inet/networklayer/common/L3Address.h"

using namespace s2f::p2p;
using namespace inet;

Packet *PowMessageBuilder::buildMessage(enum Command c)
{
    switch (c)
    {
    case VERSION:
        return buildVersionMessage();
    case VERACK:
        return buildVerackMessage();
    case GETADDR:
        return buildGetaddrMessage();
    case ADDR:
        return buildAddrMessage();
    default:
        return nullptr;
    }
}

Packet *PowMessageBuilder::buildVersionMessage()
{
    auto packet = new Packet("Version message");
    auto header = makeShared<PowMsgHeader>();
    auto payload = makeShared<PowMsgVersion>();

    // Message Header
    header->setCommandName("version");
    header->setStartString(StartString::MAIN_NET);
    header->setPayloadSize(0);
    header->setChecksum("TODO");

    // Version Message Payload
    payload->setVersion(1);
    payload->setServices(PowServiceType::NODE_NETWORK);
    payload->setTimestamp(0);

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

    packet->insertAtBack(header);
    packet->insertAtBack(payload);
    return packet;
}

Packet *PowMessageBuilder::buildVerackMessage()
{
    auto packet = new Packet("Verack message");
    auto header = makeShared<PowMsgHeader>();

    // Message Header
    header->setCommandName("verack");
    header->setStartString(StartString::MAIN_NET);
    header->setPayloadSize(0);
    header->setChecksum("TODO");

    packet->insertAtBack(header);
    return packet;
}

Packet *PowMessageBuilder::buildGetaddrMessage()
{
    auto packet = new Packet("GetAddr message");
    auto header = makeShared<PowMsgHeader>();

    // Message Header
    header->setCommandName("getaddr");
    header->setStartString(StartString::MAIN_NET);
    header->setPayloadSize(0);
    header->setChecksum("TODO");

    packet->insertAtBack(header);
    return packet;
}

Packet *PowMessageBuilder::buildAddrMessage()
{
    auto packet = new Packet("Addr message");
    auto header = makeShared<PowMsgHeader>();
    auto payload = makeShared<PowMsgAddress>();

    PowNetworkPeer peer;

    peer.port = 8333;
    peer.time = 0;
    peer.ipAddress = L3Address("10.0.0.5");
    peer.services = PowServiceType::NODE_NETWORK;

    // Message Header
    header->setCommandName("addr");
    header->setStartString(StartString::MAIN_NET);
    header->setPayloadSize(0);
    header->setChecksum("TODO");

    // Payload
    payload->setIpAddressCount(1);
    payload->appendIpAddresses(peer);

    packet->insertAtBack(header);
    packet->insertAtBack(payload);
    return packet;
}
