#include "base.h"
#include <openssl/crypto.h>
#include <openssl/x509.h>

namespace s2f::os::crypto
{
    void append(const void *src, bytes &dst, size_t len)
    {
        auto bytes = static_cast<const std::byte *>(src);
        dst.insert(dst.end(), bytes, bytes + len);
    }

    bytes toBytes(const void *buf, size_t size)
    {
        auto buffer = static_cast<const std::byte *>(buf);
        return bytes(buffer, buffer + size);
    }
}
