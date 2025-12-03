#include "FullNode.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/regmacros.h"
#include "s2f/architecture/blockchain/pow/data/Block.h"
#include "s2f/architecture/blockchain/pow/data/Blockchain.h"
#include "s2f/architecture/blockchain/pow/data/Transaction.h"
#include "s2f/os/crypto/crypto.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <sys/types.h>

using namespace s2f::chain::pow;
using namespace s2f::os::crypto;
using namespace omnetpp;

Define_Module(FullNode);

// ------------------------------------------------------------------------- //
//                              DUMMY TESTS                                  //
// ------------------------------------------------------------------------- //
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

void FullNode::dummyCryptoApiTest()
{
    // Serialize and deserialize
    auto ser = serializePublic(priv);
    key pub = deserializePublic(ser);

    // Sign and verify -- True
    EV << "Verifying signed serialization: ";
    auto signature = sign(priv, ser);
    EV << std::boolalpha << verify(pub, ser, signature) << "\n";
    assert(verify(pub, ser, signature) == true);

    // Alter and verify -- False
    EV << "Verifying altered signature: ";
    signature[0] = (std::byte)0;
    EV << std::boolalpha << verify(pub, ser, signature) << "\n";
    assert(verify(pub, ser, signature) == false);

    Block b(nullptr, GENESIS_NBITS);

    EV << "Trying sha256 hash: ";
    auto sha = sha256(ser.data(), ser.size());
    printHex(sha);
    assert(sha.size() == SHA256_DIGEST_LENGTH);

    EV << "Trying ripemd160 hash: ";
    auto ripemd = ripemd160(ser.data(), ser.size());
    printHex(ripemd160(ser.data(), ser.size()));
    assert(ripemd.size() == RIPEMD160_DIGEST_LENGTH);
}

void FullNode::dummyBlockCreationTest()
{
    // Add fake funds for the chain
    Transaction funds, funds2;
    Transaction spending, spending2;

    EV << "Adding fake funds for the chain...\n";
    funds.addOutput(3009, pubDer);
    utxo.add(funds);

    funds2.addOutput(3001, pubDer);
    utxo.add(funds2);

    EV << "Adding a transaction to the mempool...\n";
    spending.addInput(funds.hash(), 0, 3009, priv, pubDer);
    addToMempool(spending);

    spending2.addInput(funds2.hash(), 0, 3001, priv, pubDer);
    addToMempool(spending2);

    assert(mempool.top().fee == 3009);
    return;
}

// ------------------------------------------------------------------------- //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                              THE MODULE ITSELF                            //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
// ------------------------------------------------------------------------- //

void FullNode::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    blockchain.clear();

    priv = createKeyPair();
    pubDer = serializePublic(priv);

    cMessage *msg = new cMessage("btcstartup");
    send(msg, "out");
}

void FullNode::handleMessage(omnetpp::cMessage *msg)
{
    delete msg;

    dummyCryptoApiTest();
    dummyBlockCreationTest();
    Block b = mineBlock();

    addBlock(b);
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

    // TODO: change hard-coding for configurable value
    sha256digest target = block.header.getTarget(2);

    // PERF: PoW with branch prediction optimization
    while (gt(block.hash(), target))
        if (__builtin_expect(++block.header.nonce == 0, false))
            block.header.time = time(nullptr);

    // TODO: send to chain after completing PoW
    // TODO: add 10 minutes of execution time on CPU
    return block;
}

void FullNode::addCoinbase(Block &block)
{

    uint64_t fee = 0;
    int chainSize = blockchain.size();
    Transaction coinbase;

    for (const auto &txfee : block.transactions)
        fee += txfee.fee;

    // TODO: sequenceNumber/witness
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

// FIXME: Vulnerable against double-spending
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

// FIXME: Vulnerable against double-spending
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

void FullNode::addToMempool(Transaction tx)
{
    uint64_t fee;

    if ((fee = validateTransaction(tx)) < 0)
        return;

    // TODO: add BIP125 compatibility
    // If a new transaction with the same inputs presents a higher fee,
    // remove the old one and insert the new one

    mempool.push(TxFee{.tx = tx, .fee = fee});
    EV_DEBUG << "Added transaction with fee of " << fee << "\n";
}
