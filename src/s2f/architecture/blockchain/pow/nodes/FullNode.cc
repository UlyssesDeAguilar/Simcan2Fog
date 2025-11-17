#include "FullNode.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/regmacros.h"
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sys/types.h>

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
    EV << "\n";
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
        .outputs = {{.amount = 10}, {.amount = 15}},
        .inputs = {},
        .locktime = 1};

    printHex(b.hash());
    printHex(t.hash());
    EV << t.size() << "\n";

    for (int i = 0; i < 5; i++)
    {
        t.version = i;
        utxo.add(t);
        b.add(t);
    }

    printHex(b.merkleRoot());

    for (int i = 0; i < 5; i++)
    {
        EV << utxo.getCoin(b.transactions[i].hash(), 0) << "\n";
        EV << utxo.getCoin(b.transactions[i].hash(), 1) << "\n";
        utxo.spendCoin(b.transactions[i].hash(), 0);
        utxo.spendCoin(b.transactions[i].hash(), 1);
    }

    return;
}

void FullNode::mineBlock()
{

    Block &block = blockchain.back();
    sha256digest target = block.header.getTarget();

    addCoinbase();

    // TODO: add "fakeness" factor + execute()
    // Ensures simulation time is around ~10 mins for block
    // While keeping real time on only a few seconds at most

    // Proof-of-Work: Mine nonce until hash <= target
    while (std::memcmp(block.hash().data(), target.data(), target.size()) > 0)
        if (++block.header.nonce == 0)
            block.header.time = time(nullptr);
}

void FullNode::addCoinbase()
{
    Block &block = blockchain.back();

    uint64_t fee = 0;
    uint32_t BLOCK_SUBSIDY = 3125000000;

    for (const auto &t : block.transactions)
    {
        auto txid = t.hash();

        for (const auto &i : t.inputs)
            fee += utxo.getCoin(txid, i.vout);

        for (const auto &o : t.outputs)
            fee -= o.amount;
    }

    // TODO: Compute BLOCK_SUBSIDY instead of hardcoded
    // TODO: version, pubkeyScript, signatureScript, pubkeyScript, sequenceNumber

    // Create the transaction
    Transaction coinbase{
        .version = 1,
        .outputs = {{.amount = fee + BLOCK_SUBSIDY, .pubkeyScript = {}}},
        .inputs = {{.txid = {}, .vout = std::numeric_limits<uint32_t>::max(), .signatureScript = {}, .sequenceNumber = 0}},
        .locktime = 100,
    };

    block.transactions.insert(block.transactions.begin(), coinbase);
    block.header.merkleRootHash = block.merkleRoot();
}
