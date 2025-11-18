#include "Transaction.h"
#include "s2f/os/crypto/crypto.h"

using namespace s2f::chain::pow;
using namespace s2f::os::crypto;

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
