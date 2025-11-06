#include "FullNode.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/regmacros.h"
#include "s2f/architecture/blockchain/pow/Block.h"
#include <cstddef>
#include <iomanip>

using namespace s2f::chain::pow;
using namespace omnetpp;

Define_Module(FullNode);

template <typename Container>
void printHex(const Container &bytes)
{
    EV << std::hex;
    for (const auto &byte : bytes)
    {
        EV
            << std::setw(2)
            << std::setfill('0')
            << std::hex
            << std::to_integer<uint32_t>(byte)
            << "";
    }
    EV << std::dec;
}

void FullNode::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    blockchain.clear();

    cMessage *msg = new cMessage("btcstartup");
    send(msg, "out");
}

void FullNode::handleMessage(omnetpp::cMessage *msg)
{
    EV << "This has started!!" << "\n";
    delete msg;

    Block b;
    Transaction t;

    b.header = {
        .version = 1,
        .parentBlockHash = {},
        .merkleRootHash = {},
        .time = 0,
        .nBits = 1,
        .nonce = 1};

    t = {
        .version = 0,
        .outputs = {},
        .inputs = {},
        .locktime = 1};

    printHex(b.hash());

    b.add(t);

    return;
}
