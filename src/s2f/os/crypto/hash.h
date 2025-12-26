#ifndef __SIMCAN_CRYPTO_HASH_H__
#define __SIMCAN_CRYPTO_HASH_H__

#include "base.h"
#include <array>
#include <cstring>
#include <openssl/x509.h>
#include <vector>

namespace s2f::os::crypto
{

    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------
    constexpr size_t RIPEMD160_DIGEST_LENGTH = 20; //<! Size of ripemd digest

    //-------------------------------------------------------------------------
    // Data types
    //-------------------------------------------------------------------------

    /* Digest for SHA256 */
    using sha256digest = std::array<std::byte, SHA256_DIGEST_LENGTH>;

    /* Digest for RipeMD160 */
    using ripemd160digest = std::array<std::byte, RIPEMD160_DIGEST_LENGTH>;

    //-------------------------------------------------------------------------
    // Methods
    //-------------------------------------------------------------------------

    /**
     * Computes a ripemd160 hash of the given data.
     *
     * Mostly used to reduce public key size in bitcoin.
     *
     * @param data Data to hash.
     * @param size Data size.
     *
     * @return The hash value as a std::array.
     */
    template <typename T>
    ripemd160digest ripemd160(const T *data, size_t size)
    {
        ripemd160digest hash;
        EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
        unsigned int md_len = RIPEMD160_DIGEST_LENGTH;
        const EVP_MD *md = EVP_get_digestbyname("RIPEMD160");

        auto d = reinterpret_cast<const unsigned char *>(data);
        auto h = reinterpret_cast<unsigned char *>(hash.data());

        EVP_DigestInit_ex2(mdctx, md, nullptr);
        EVP_DigestUpdate(mdctx, d, size);
        EVP_DigestFinal_ex(mdctx, h, &md_len);

        EVP_MD_CTX_free(mdctx);
        return hash;
    }

    /** * Computes a sha256 hash of the given data.
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
     * Computes the double sha256 hash of the given data.
     *
     * @param data Data to hash.
     * @param size Data size.
     *
     * @return The hash value.
     */
    template <typename T>
    sha256digest dsha256(const T *data, size_t size)
    {
        return sha256(sha256(data, size).data(), SHA256_DIGEST_LENGTH);
    }

    inline ripemd160digest hashPublic(bytes &pubDer)
    {
        return ripemd160(sha256(pubDer.data(), pubDer.size()).data(), SHA256_DIGEST_LENGTH);
    }

    inline bool gt(sha256digest a, sha256digest b)
    {
        return std::memcmp(a.data(), b.data(), b.size()) > 0;
    }
}
#endif
