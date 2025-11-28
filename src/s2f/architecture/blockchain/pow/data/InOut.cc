#include "InOut.h"
#include "s2f/os/crypto/crypto.h"
#include <algorithm>

using namespace s2f::chain::pow;
using namespace s2f::os::crypto;

bytes utxo::getBytes() const
{
    bytes buf;
    buf.reserve(size());

    append(&amount, buf, sizeof(amount));
    append(pubkeyScript.data(), buf, pubkeyScript.size());

    return buf;
}

void utxo::getPubkeyScript(ripemd160digest &pubHash) const
{
    std::copy_n(pubkeyScript.begin(), RIPEMD160_DIGEST_LENGTH, pubHash.begin());
}

void utxo::buildPubkeyScript(bytes &pubDer)
{
    ripemd160digest pubHash = hashPublic(pubDer);
    pubkeyScript = toBytes(pubHash.data(), pubHash.size());
}
bytes txi::getBytes() const
{
    bytes buf;
    buf.reserve(size());

    append(txid.data(), buf, txid.size());
    append(&vout, buf, sizeof(vout));
    append(signatureScript.data(), buf, signatureScript.size());
    append(&sequenceNumber, buf, sizeof(sequenceNumber));

    return buf;
}

void txi::buildSignatureScript(uint64_t amount, const key &priv, const bytes &pubDer)
{
    signatureScript.clear();

    auto signatureData = toBytes(&amount, sizeof(amount));
    auto signature = sign(priv, signatureData);

    append(signature.data(), signatureScript, signature.size());
    append(pubDer.data(), signatureScript, pubDer.size());
}

void txi::getSignatureScript(bytes &signature, bytes &pubDer, key &pub) const
{
    signature.assign(signatureScript.begin(), signatureScript.begin() + SIGSIZE);
    pubDer.assign(signatureScript.begin() + SIGSIZE, signatureScript.end());
    pub = deserializePublic(pubDer);
}
