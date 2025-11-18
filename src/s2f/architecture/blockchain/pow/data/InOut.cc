#include "InOut.h"

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
