#include "crypto.h"
#include <openssl/crypto.h>
#include <openssl/x509.h>

namespace s2f::os::crypto
{

    void append(const void *src, bytes &dst, size_t len)
    {
        auto bytes = static_cast<const std::byte *>(src);
        dst.insert(dst.end(), bytes, bytes + len);
    }

    unsigned char *toUnsigned(bytes &buf)
    {
        return reinterpret_cast<unsigned char *>(buf.data());
    }

    bytes toBytesOld(unsigned char *buf, size_t size)
    {
        return bytes(reinterpret_cast<std::byte *>(buf),
                     reinterpret_cast<std::byte *>(buf) + size);
    }

    bytes toBytes(const void *buf, size_t size)
    {
        auto buffer = static_cast<const std::byte *>(buf);
        return bytes(buffer, buffer + size);
    }

    key createKeyPair()
    {
        EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_name(NULL, "ED25519", NULL);
        EVP_PKEY *pk = nullptr;
        key priv{nullptr};

        // Initialize generator
        if (EVP_PKEY_keygen_init(ctx) <= 0)
        {
            EVP_PKEY_CTX_free(ctx);
            return nullptr;
        }

        // Initialize
        if (EVP_PKEY_keygen(ctx, &pk) <= 0)
        {
            EVP_PKEY_CTX_free(ctx);
            return nullptr;
        }

        priv.reset(pk);

        return priv;
    }

    bytes serializePublic(const key &key)
    {
        unsigned char *p = nullptr;
        int len = i2d_PUBKEY(key.get(), &p);

        bytes buffer = toBytes(p, len);
        OPENSSL_free(p);

        return buffer;
    }

    key deserializePublic(bytes &data)
    {
        const unsigned char *p = toUnsigned(data);
        EVP_PKEY *pb = nullptr;
        key pub{nullptr};

        d2i_PUBKEY(&pb, &p, data.size());
        pub.reset(pb);
        return pub;
    }

    bytes sign(const key &priv, bytes &data)
    {
        size_t siglen = SIGSIZE;
        unsigned char *buf = static_cast<unsigned char *>(OPENSSL_zalloc(siglen));
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        bytes signature;

        // Load and sign data
        EVP_DigestSignInit(ctx, NULL, NULL, NULL, priv.get());
        EVP_DigestSign(ctx, buf, &siglen, toUnsigned(data), data.size());

        signature = toBytes(buf, siglen);
        EVP_MD_CTX_free(ctx);
        OPENSSL_free(buf);

        return signature;
    }

    bool verify(const key &pub, bytes &data, bytes &signature)
    {
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        int ret;

        // Initialize verification context
        EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pub.get());
        ret = EVP_DigestVerify(ctx, toUnsigned(signature), SIGSIZE, toUnsigned(data), data.size());
        EVP_MD_CTX_free(ctx);

        return ret == 1;
    }

}
