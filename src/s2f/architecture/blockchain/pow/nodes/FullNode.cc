#include "FullNode.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/regmacros.h"
#include <algorithm>
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
    }

    printHex(b.merkleRoot());

    for (int i = 0; i < 5; i++)
    {
        EV << utxo.getCoin(b.transactions[i].t.hash(), 0) << "\n";
        EV << utxo.getCoin(b.transactions[i].t.hash(), 1) << "\n";
        utxo.spendCoin(b.transactions[i].t.hash(), 0);
        utxo.spendCoin(b.transactions[i].t.hash(), 1);
    }

    return;
}

void FullNode::mineBlock(size_t limit)
{

    // TODO: create new block instead
    Block &block = blockchain.back();
    sha256digest target = block.header.getTarget();

    // Add transactions + coinbase
    for (int i = 0; i < std::min(limit, mempool.size()); i++)
    {
        block.transactions.push_back(mempool.top());
        mempool.pop();
    }
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

    for (const auto &txfee : block.transactions)
        fee += txfee.fee;

    // TODO: Compute BLOCK_SUBSIDY instead of hardcoded
    // TODO: version, pubkeyScript, signatureScript, pubkeyScript, sequenceNumber

    // Create the transaction
    Transaction coinbase{
        .version = 1,
        .outputs = {{.amount = fee + BLOCK_SUBSIDY, .pubkeyScript = {}}},
        .inputs = {{.txid = {}, .vout = std::numeric_limits<uint32_t>::max(), .signatureScript = {}, .sequenceNumber = 0}},
        .locktime = 100,
    };

    block.transactions.insert(block.transactions.begin(), TxFee{.t = coinbase});
    block.header.merkleRootHash = block.merkleRoot();
}

void FullNode::addToMempool(Transaction t)
{
    uint64_t fee = 0;
    uint64_t amount;

    // Compute the fee based on inputs and outputs
    // TODO: script validation
    for (const auto &i : t.inputs)
    {
        if ((amount = utxo.getCoin(i.txid, i.vout)) == -1)
            return;

        fee += amount;
    }

    for (const auto &o : t.outputs)
        fee -= o.amount;

    if (fee < 0)
        return;

    // TODO: add BIP125 compatibility
    // If a new transaction with the same inputs presents a higher fee,
    // remove the old one and insert the new one
    mempool.push(TxFee{.t = t, .fee = fee});
}

void FullNode::addToMempool(std::vector<Transaction> &trns)
{
    for (auto t : trns)
        addToMempool(t);
}
