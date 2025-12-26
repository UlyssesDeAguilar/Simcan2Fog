#ifndef __SIMCAN_CRYPTO_KEYPAIR_H__
#define __SIMCAN_CRYPTO_KEYPAIR_H__

#include "base.h"
#include <array>
#include <cstring>
#include <memory>
#include <openssl/evp.h>
#include <vector>

namespace s2f::os::crypto
{

    //-------------------------------------------------------------------------
    // Constants
    //-------------------------------------------------------------------------
    constexpr size_t SIGSIZE = 64; //<! Signature size on Ed25519

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
}
#endif
