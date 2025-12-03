#include "Transaction.h"
#include "s2f/os/crypto/crypto.h"

using namespace s2f::chain::pow;
using namespace s2f::os::crypto;

Transaction Transaction::fromJSON(std::string data)
{
    // TODO
    return *this;
}

Transaction::Transaction()
{
    version = 1;
    locktime = 100;
}

void Transaction::addOutput(uint64_t amount, bytes &pubDer)
{
    struct utxo o = {.amount = amount};
    ripemd160digest pubHash = hashPublic(pubDer);

    o.pubkeyScript = toBytes(pubHash.data(), pubHash.size());

    outputs.push_back(o);
}

void Transaction::addInput(sha256digest txid, uint32_t vout, uint64_t amount, key &priv, const bytes &pubDer)
{
    struct txi i = {
        .txid = txid,
        .vout = vout,
        .sequenceNumber = 0,
    };

    bytes signatureData = toBytes(&amount, sizeof(amount));
    bytes signature = sign(priv, signatureData);

    append(signature.data(), i.signatureScript, signature.size());
    append(pubDer.data(), i.signatureScript, pubDer.size());

    inputs.push_back(i);
}

sha256digest Transaction::hash() const
{
    bytes buf;
    buf.reserve(size());

    append(&version, buf, sizeof(version));

    for (const auto &o : outputs)
        append(o.getBytes().data(), buf, o.size());

    for (const auto &i : inputs)
        append(i.getBytes().data(), buf, i.size());

    append(&locktime, buf, sizeof(locktime));

    return sha256(buf.data(), buf.size());
}

size_t Transaction::size() const
{
    size_t s = sizeof(version) + sizeof(locktime);

    for (const auto &i : inputs)
        s += i.size();

    for (const auto &o : outputs)
        s += o.size();

    return s;
}
