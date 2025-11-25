#include "FullNode.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/regmacros.h"
#include "s2f/architecture/blockchain/pow/data/Blockchain.h"
#include "s2f/os/crypto/crypto.h"
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <openssl/pem.h>
#include <sys/types.h>

using namespace s2f::chain::pow;
using namespace s2f::os::crypto;
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
    delete msg;
    key priv;

    // Create key
    priv = createKeyPair();

    // Serialize and deserialize
    auto ser = serializePublic(priv);
    key pub = deserializePublic(ser);

    // Sign and verify -- True
    auto signature = sign(priv, ser);
    EV << std::boolalpha << verify(pub, ser, signature) << "\n";

    // Alter and verify -- False
    signature[0] = (std::byte)0;
    EV << std::boolalpha << verify(pub, ser, signature) << "\n";

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

    sha256digest target = block.header.getTarget();

    // TODO: add "fakeness" factor + execute()
    // Ensures simulation time is around ~10 mins for block
    // While keeping real time on only a few seconds at most

    // Proof-of-Work: Mine nonce until hash <= target
    while (std::memcmp(block.hash().data(), target.data(), target.size()) > 0)
        if (++block.header.nonce == 0)
            block.header.time = time(nullptr);

    // TODO: send to chain after completing PoW
}

void FullNode::addCoinbase(Block &block)
{

    uint64_t fee = 0;

    for (const auto &txfee : block.transactions)
        fee += txfee.fee;

    // TODO: version, pubkeyScript, signatureScript, pubkeyScript, sequenceNumber
    Transaction coinbase{
        .version = 1,
        .outputs = {{.amount = fee + getSubsidy(blockchain.size()), .pubkeyScript = {}}},
        .inputs = {{.txid = {}, .vout = std::numeric_limits<uint32_t>::max(), .signatureScript = {}, .sequenceNumber = 0}},
        .locktime = 100,
    };

    block.transactions.insert(block.transactions.begin(), TxFee{.tx = coinbase});
    block.header.merkleRootHash = block.merkleRoot();
}

void FullNode::addToMempool(Transaction tx)
{
    uint64_t fee = 0;
    uint64_t amount;

    // Compute the fee based on inputs and outputs
    // TODO: script validation
    for (const auto &i : tx.inputs)
    {
        if ((amount = utxo.getCoin(i.txid, i.vout)) == -1)
            return;

        fee += amount;
    }

    for (const auto &o : tx.outputs)
        fee -= o.amount;

    if (fee < 0)
        return;

    // TODO: add BIP125 compatibility
    // If a new transaction with the same inputs presents a higher fee,
    // remove the old one and insert the new one
    mempool.push(TxFee{.tx = tx, .fee = fee});
}
