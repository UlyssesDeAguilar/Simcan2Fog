#include "FullNode.h"
#include "../chain/Blockchain.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/regmacros.h"
#include "s2f/os/crypto/hash.h"
#include "s2f/os/crypto/keypair.h"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sys/types.h>

using namespace s2f::p2p::pow;
using namespace s2f::os::crypto;
using namespace omnetpp;

Define_Module(FullNode);

void FullNode::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    blockchain.clear();

    priv = createKeyPair();
    pubDer = serializePublic(priv);
}

void FullNode::handleMessage(omnetpp::cMessage *msg)
{
    delete msg;
    return;
}

Block FullNode::mineBlock()
{

    Block block(last(), getDifficulty(blockchain, blockchain.size() - 1));
    size_t blockSize = sizeof(BlockHeader);

    while (blockSize < MAX_BLOCK_SERIALIZED_SIZE - COINBASE_SIZE && mempool.size())
    {
        block.transactions.push_back(mempool.top());
        blockSize += mempool.top().tx.size();
        mempool.pop();
    }
    addCoinbase(block);

    sha256digest target = block.header.getTarget(2);

    // PERF: PoW with branch prediction optimization
    while (gt(block.hash(), target))
        if (__builtin_expect(++block.header.nonce == 0, false))
            block.header.time = time(nullptr);

    return block;
}

void FullNode::addBlock(const Block b)
{
    const Transaction &coinbase = b.transactions.front().tx;
    int chainSize = blockchain.size();
    uint64_t fee = getSubsidy(chainSize);
    sha256digest t = b.header.getTarget(2);
    uint32_t nBits = getDifficulty(blockchain, blockchain.size() - 1);

    // Verify merkle and parent
    if (!b.isValid(last()))
        return;

    // Verify target was reached
    if (nBits != b.header.nBits || gt(b.hash(), t))
        return;

    // Validate transactions and recompute fee
    for (auto it = std::next(b.transactions.begin()); it != b.transactions.end(); it++)
    {
        const auto &t = *it;

        if (t.fee < 0 || t.fee != validateTransaction(t.tx))
            return;

        fee += t.fee;
    }

    // Validate the coinbase transaction
    if (coinbase.inputs.size() != 1 || coinbase.outputs.empty())
        return;

    const struct txi &input = coinbase.inputs[0];
    const std::vector<struct utxo> &outputs = coinbase.outputs;

    //  1. Input format
    if (input.txid != GENESIS_HASH || input.vout != std::numeric_limits<uint32_t>::max())
        return;

    if (input.signatureScript != toBytes(&chainSize, sizeof(chainSize)))
        return;

    //  2. Output fee
    for (const auto &o : outputs)
        fee -= o.amount;

    if (fee != 0)
        return;

    // Update utxo set and add block
    for (auto it = std::next(b.transactions.begin()); it != b.transactions.end(); it++)
        for (auto &i : (*it).tx.inputs)
            utxo.spendCoin(i.txid, i.vout);

    utxo.add(coinbase);
    blockchain.push_back(b);

    EV_DEBUG << "Added block with index " << chainSize << " to the chain\n";
}

void FullNode::addCoinbase(Block &block)
{

    uint64_t fee = 0;
    int chainSize = blockchain.size();
    Transaction coinbase;

    for (const auto &txfee : block.transactions)
        fee += txfee.fee;

    coinbase.addOutput(fee + getSubsidy(blockchain.size()), pubDer);
    coinbase.addInput({
        .txid = {},
        .vout = std::numeric_limits<uint32_t>::max(),
        .signatureScript = toBytes(&chainSize, sizeof(chainSize)),
        .sequenceNumber = 0,
    });

    block.transactions.insert(block.transactions.begin(), TxFee{.tx = coinbase});
    block.header.merkleRootHash = block.merkleRoot();
}

void FullNode::addToMempool(Transaction tx)
{
    uint64_t fee;

    if ((fee = validateTransaction(tx)) < 0)
        return;

    mempool.push(TxFee{.tx = tx, .fee = fee});
    EV_DEBUG << "Added transaction with fee of " << fee << "\n";
}

int FullNode::validateTransaction(const Transaction &tx) const
{
    uint64_t fee = 0;
    const struct utxo *out;

    key inputPub;
    bytes inputSignature, inputPubDer;

    ripemd160digest outputPubHash;
    bytes outputSignature;

    // Compute the fee based on inputs and outputs
    for (const auto &i : tx.inputs)
    {
        if ((out = utxo.get(i.txid, i.vout)) == nullptr || out->amount == -1)
        {
            EV_DEBUG << "Could not find unspent outpoint.\n";
            return -1;
        }

        // Extract script data
        i.getSignatureScript(inputSignature, inputPubDer, inputPub);
        out->getPubkeyScript(outputPubHash);

        // Verify fields
        outputSignature = toBytes(&out->amount, sizeof(out->amount));

        if (hashPublic(inputPubDer) != outputPubHash ||
            verify(inputPub, outputSignature, inputSignature) == false)
        {
            EV_DEBUG << "P2PKH validation failed.\n";
            return -1;
        }

        fee += out->amount;
    }

    for (const auto &o : tx.outputs)
        fee -= o.amount;

    return fee;
}
