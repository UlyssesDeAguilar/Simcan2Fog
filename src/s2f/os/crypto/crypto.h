#ifndef __SIMCAN_CRYPTO_H__
#define __SIMCAN_CRYPTO_H__

#include <array>
#include <openssl/sha.h>

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
    std::array<std::byte, SHA256_DIGEST_LENGTH> sha256(const T *data, const int size)
    {
        std::array<std::byte, SHA256_DIGEST_LENGTH> hash;
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
    std::array<std::byte, SHA256_DIGEST_LENGTH> dsha256(const T *data, const int size)
    {
        return sha256(sha256(data, size).data(), SHA256_DIGEST_LENGTH);
    }
}
#endif
