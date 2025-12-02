#include "FullNode.h"
#include "omnetpp/clog.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/regmacros.h"
#include "s2f/architecture/blockchain/pow/data/Block.h"
#include "s2f/architecture/blockchain/pow/data/Blockchain.h"
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

    Block b;

    b.header = {
        .version = 1,
        .parentBlockHash = {},
        .merkleRootHash = {},
        .time = static_cast<uint32_t>(time(nullptr)),
        .nBits = 0x1dffffff,
        .nonce = 0,
    };

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
    bytes pubDer = serializePublic(priv);

    // Add fake funds for the chain
    EV << "Adding fake funds for the chain...\n";
    Transaction funds = {
        .version = 1,
        .outputs = {
            {
                .amount = 3009,
                .pubkeyScript = {},
            }},
        .inputs = {},
        .locktime = 100,
    };

    funds.outputs[0].buildPubkeyScript(pubDer);

    utxo.add(funds);

    auto funds2 = funds;
    funds2.outputs[0].amount = 3001;

    utxo.add(funds2);

    auto test = utxo.get(funds.hash(), 0);
    EV << test->amount << "\n";

    EV << "Adding a transaction to the mempool...\n";
    // Add a transaction
    Transaction spending = {
        .version = 1,
        .outputs = {}, // Pay all to miner
        .inputs = {{
            .txid = funds.hash(),
            .vout = 0,
            .signatureScript = {},
            .sequenceNumber = 0,
        }},
        .locktime = 100,
    };

    // Sign transaction
    spending.inputs[0].buildSignatureScript(3009, priv, pubDer);

    Transaction spending2 = {
        .version = 1,
        .outputs = {}, // Pay all to miner
        .inputs = {{
            .txid = funds2.hash(),
            .vout = 0,
            .signatureScript = {},
            .sequenceNumber = 0,
        }},
        .locktime = 100,
    };

    spending2.inputs[0].buildSignatureScript(3001, priv, pubDer);

    addToMempool(spending);
    addToMempool(spending2);

    assert(mempool.top().fee == 3009);
    return;
}

// ------------------------------------------------------------------------- //
//                              THE MODULE ITSELF                            //
// ------------------------------------------------------------------------- //
void FullNode::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    blockchain.clear();

    priv = createKeyPair();

    cMessage *msg = new cMessage("btcstartup");
    send(msg, "out");
}

void FullNode::handleMessage(omnetpp::cMessage *msg)
{
    delete msg;

    dummyCryptoApiTest();
    dummyBlockCreationTest();
    return;
}

void FullNode::mineBlock()
{

    Block block;
    size_t blockSize = sizeof(BlockHeader);

    block.header = {
        .version = 1,
        .parentBlockHash = blockchain.back().hash(),
        .merkleRootHash = {},
        .time = static_cast<uint32_t>(time(nullptr)),
        .nBits = getDifficulty(blockchain, blockchain.size() - 1),
        .nonce = 0,
    };

    while (blockSize < MAX_BLOCK_SERIALIZED_SIZE - COINBASE_SIZE && mempool.size())
    {
        block.transactions.push_back(mempool.top());
        blockSize += mempool.top().tx.size();
        mempool.pop();
    }
    addCoinbase(block);

    sha256digest target = block.header.getTarget(999);

    // Proof-of-Work: Mine nonce until hash <= target
    // PERF: favors false scenario in branch prediction
    while (std::memcmp(block.hash().data(), target.data(), target.size()) > 0)
        if (__builtin_expect(++block.header.nonce == 0, false))
            block.header.time = time(nullptr);

    // TODO: send to chain after completing PoW
    // TODO: add 10 minutes of execution time on CPU
}

void FullNode::addCoinbase(Block &block)
{

    uint64_t fee = 0;
    bytes pubDer;
    ripemd160digest outputHash;
    int chainSize = blockchain.size();

    for (const auto &txfee : block.transactions)
        fee += txfee.fee;

    // TODO: version, sequenceNumber/witness
    Transaction coinbase{
        .version = 1,
        .outputs = {{.amount = fee + getSubsidy(blockchain.size()), .pubkeyScript = {}}},
        .inputs = {{.txid = {}, .vout = std::numeric_limits<uint32_t>::max(), .signatureScript = toBytes(&chainSize, sizeof(chainSize)), .sequenceNumber = 0}},
        .locktime = 100,
    };

    pubDer = serializePublic(priv);
    outputHash = ripemd160(sha256(pubDer.data(), pubDer.size()).data(), SHA256_DIGEST_LENGTH);

    coinbase.outputs[0].pubkeyScript = toBytes(outputHash.data(), outputHash.size());

    block.transactions.insert(block.transactions.begin(), TxFee{.tx = coinbase});
    block.header.merkleRootHash = block.merkleRoot();
}

void FullNode::addToMempool(Transaction tx)
{
    uint64_t fee = 0;
    const struct utxo *out;

    key pub;
    ripemd160digest pubHash;
    bytes signature, pubDer, signedData;

    // Compute the fee based on inputs and outputs
    for (const auto &i : tx.inputs)
    {
        if ((out = utxo.get(i.txid, i.vout)) == nullptr || out->amount == -1)
        {
            EV_DEBUG << "Could not find unspent outpoint.\n";
            return;
        }

        // Extract script data
        i.getSignatureScript(signature, pubDer, pub);
        out->getPubkeyScript(pubHash);

        // Verify fields
        signedData = toBytes(&out->amount, sizeof(out->amount));
        if (hashPublic(pubDer) != pubHash || verify(pub, signedData, signature) == false)
        {
            EV_DEBUG << "P2PKH validation failed.\n";
            return;
        }

        fee += out->amount;
    }

    for (const auto &o : tx.outputs)
        fee -= o.amount;

    if (fee < 0)
        return;

    // TODO: add BIP125 compatibility
    // If a new transaction with the same inputs presents a higher fee,
    // remove the old one and insert the new one

    mempool.push(TxFee{.tx = tx, .fee = fee});
    EV_DEBUG << "Added transaction with fee of " << fee << "\n";
}
