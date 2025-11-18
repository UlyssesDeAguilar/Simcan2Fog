#include "crypto.h"

void appendBytes(const void *src, bytes &dst, size_t len)
{
    auto bytes = static_cast<const std::byte *>(src);
    dst.insert(dst.end(), bytes, bytes + len);
}
