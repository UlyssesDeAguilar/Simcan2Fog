#include "InOut.h"
#include <algorithm>

using namespace s2f::p2p::pow;
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
    assert(pubkeyScript.size() >= RIPEMD160_DIGEST_LENGTH);

    std::copy_n(pubkeyScript.begin(), RIPEMD160_DIGEST_LENGTH, pubHash.begin());
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

void txi::getSignatureScript(bytes &signature, bytes &pubDer, key &pub) const
{
    assert(signatureScript.size() > SIGSIZE);

    signature.assign(signatureScript.begin(), signatureScript.begin() + SIGSIZE);
    pubDer.assign(signatureScript.begin() + SIGSIZE, signatureScript.end());
    pub = deserializePublic(pubDer);
}
