#ifndef __SIMCAN_CRYPTO_H__
#define __SIMCAN_CRYPTO_H__

#include "omnetpp/clog.h"
#include <array>
#include <memory>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/x509.h>
#include <vector>

namespace s2f::os::crypto
{

    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------
    constexpr size_t SIGSIZE = 64;                 //<! Signature size on Ed25519
    constexpr size_t RIPEMD160_DIGEST_LENGTH = 20; //<! Size of ripemd digest

    //-------------------------------------------------------------------------
    // Data types
    //-------------------------------------------------------------------------

    /**
     * Custom destroyer for public/private keypairs
     */
    struct KeyDestroyer
    {
        void operator()(EVP_PKEY *p) const noexcept
        {
            if (p)
                EVP_PKEY_free(p);
        }
    };

    /* Digest for SHA256 */
    using sha256digest = std::array<std::byte, SHA256_DIGEST_LENGTH>;

    /* Digest for RipeMD160 */
    using ripemd160digest = std::array<std::byte, RIPEMD160_DIGEST_LENGTH>;

    /* Arbitrary byte array */
    using bytes = std::vector<std::byte>;

    /* EVP_PKEY wrapper with automatic memory management */
    using key = std::unique_ptr<EVP_PKEY, KeyDestroyer>;

    //-------------------------------------------------------------------------
    // Methods
    //-------------------------------------------------------------------------

    /**
     * Appends arbitrary data into a byte array.
     *
     * @param src   Data to append at the end of dst, which should be trivially
     *              convertible to std::byte*.
     * @param dst   Destination array.
     * @param len   Length of src.
     */
    void append(const void *src, bytes &dst, size_t len);

    /**
     * Serialize arbitrary data into a byte array.
     *
     * @param src   Data to serialize, which should be trivially
     *              convertible to std::byte*.
     * @param size  Length of src.
     */
    bytes toBytes(const void *buf, size_t size);

    /**
     * Openssl wrapper to create a private/public keypair with the ED25519
     * signature scheme.
     *
     * @return Keypair.
     */
    key createKeyPair();

    /**
     * Serialize the public component of a keypair into DER format.
     *
     * @param key   Key to serialize.
     *
     * @return Byte array DER key.
     */
    bytes serializePublic(const key &key);

    /**
     * Get a public key from a DER serialization.
     *
     * @param data   DER representation of a key.
     *
     * @return  Public key.
     */
    key deserializePublic(bytes &data);

    /**
     * Sign a buffer of data with a private key.
     *
     * @param k      Private key.
     * @param data   Buffer to sign.
     *
     * @returns Signature in bytes.
     */
    bytes sign(const key &priv, bytes &data);

    /**
     * Verify a signature against a data buffer.
     *
     * @param k         Signer public key.
     * @param data      Original data.
     * @param signature Data signed with a private key.
     *
     * @returns true if the signatures match, false otherwise.
     */
    bool verify(const key &pub, bytes &data, bytes &signature);

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
}
#endif
