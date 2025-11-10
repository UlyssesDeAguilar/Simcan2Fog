#ifndef __SIMCAN_CRYPTO_H__
#define __SIMCAN_CRYPTO_H__

#include <array>
#include <openssl/sha.h>
#include <vector>

using sha256digest = std::array<std::byte, SHA256_DIGEST_LENGTH>;
using bytes = std::vector<std::byte>;

namespace s2f::os::crypto
{
    /** * Computes a double sha256 hash of the given data.
     *
     * @param data Data to hash.
     * @param size Data size.
     *
     * @return The hash value as a std::array.
     */
    template <typename T>
    sha256digest sha256(const T *data, size_t size)
    {
        sha256digest hash;
        auto d = reinterpret_cast<const unsigned char *>(data);
        auto h = reinterpret_cast<unsigned char *>(hash.data());
        SHA256(d, size, h);
        return hash;
    }

    /**
     * Computes the sha256 hash of the given data.
     *
     * @param data Data to hash.
     * @param size Data size.
     *
     * @return The hash value as a std::array.
     */
    template <typename T>
    sha256digest dsha256(const T *data, size_t size)
    {
        return sha256(sha256(data, size).data(), SHA256_DIGEST_LENGTH);
    }
}
#endif
