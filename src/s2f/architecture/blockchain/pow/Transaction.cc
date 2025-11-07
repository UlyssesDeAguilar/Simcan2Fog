#include "Transaction.h"
#include "s2f/os/crypto/crypto.h"

using namespace s2f::chain::pow;

/**
 * Appends arbitrary data into a byte array.
 *
 * @param src   Data to append at the end of dst.
 * @param dst   Destination array.
 * @param len   Length of src.
 */
void append(const void *src, std::vector<std::byte> &dst, size_t len)
{
    const auto *bytes = static_cast<const std::byte *>(src);
    dst.insert(dst.end(), bytes, bytes + len);
}

// ------------------------------------------------------------------------- //
//                                  OUTPUTS                                  //
// ------------------------------------------------------------------------- //
std::vector<std::byte> utxo::repr() const
{
    std::vector<std::byte> repr;
    repr.reserve(size());

    append(&amount, repr, sizeof(amount));
    append(pubkeyScript.data(), repr, pubkeyScript.size());

    return repr;
}

// ------------------------------------------------------------------------- //
//                                  INPUTS                                   //
// ------------------------------------------------------------------------- //
std::vector<std::byte> txi::repr() const
{
    std::vector<std::byte> repr;
    repr.reserve(size());

    append(txid.data(), repr, txid.size());
    append(&vout, repr, sizeof(vout));
    append(signatureScript.data(), repr, signatureScript.size());
    append(&sequenceNumber, repr, sizeof(sequenceNumber));

    return repr;
}

// ------------------------------------------------------------------------- //
//                             TRANSCATION                                   //
// ------------------------------------------------------------------------- //
Transaction Transaction::fromJSON(std::string data)
{
    // TODO
    return *this;
}

sha256digest Transaction::hash()
{
    std::vector<std::byte> buf;
    buf.reserve(size());

    append(&version, buf, sizeof(version));

    for (const auto &o : outputs)
        append(o.repr().data(), buf, o.size());

    for (const auto &i : inputs)
        append(i.repr().data(), buf, i.size());

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
