#include "Transaction.h"
#include "s2f/os/crypto/crypto.h"

using namespace s2f::chain::pow;

/**
 * Appends arbitrary data into a byte array.
 *
 * @param src   Data to append at the end of dst, which should be trivially
 *              convertible to std::byte*.
 * @param dst   Destination array.
 * @param len   Length of src.
 */
void append(const void *src, bytes &dst, size_t len)
{
    auto bytes = static_cast<const std::byte *>(src);
    dst.insert(dst.end(), bytes, bytes + len);
}

// ------------------------------------------------------------------------- //
//                                  OUTPUTS                                  //
// ------------------------------------------------------------------------- //
bytes utxo::getBytes() const
{
    bytes buf;
    buf.reserve(size());

    append(&amount, buf, sizeof(amount));
    append(pubkeyScript.data(), buf, pubkeyScript.size());

    return buf;
}

// ------------------------------------------------------------------------- //
//                                  INPUTS                                   //
// ------------------------------------------------------------------------- //
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

// ------------------------------------------------------------------------- //
//                             TRANSCATION                                   //
// ------------------------------------------------------------------------- //
Transaction Transaction::fromJSON(std::string data)
{
    // TODO
    return *this;
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

    return os::crypto::sha256(buf.data(), buf.size());
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
