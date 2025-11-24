#include "crypto.h"
#include <openssl/crypto.h>
#include <openssl/evp.h>

using namespace s2f::os::crypto;

void append(const void *src, bytes &dst, size_t len)
{
    auto bytes = static_cast<const std::byte *>(src);
    dst.insert(dst.end(), bytes, bytes + len);
}

unsigned char *toUnsigned(bytes &buf)
{
    return reinterpret_cast<unsigned char *>(buf.data());
}

std::vector<std::byte> toBytes(unsigned char *buf, size_t size)
{
    return std::vector<std::byte>(reinterpret_cast<std::byte *>(buf),
                                  reinterpret_cast<std::byte *>(buf) + size);
}

key *createKeyPair()
{
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);
    key *priv;

    EVP_PKEY_keygen_init(pctx);
    EVP_PKEY_keygen(pctx, &priv);
    EVP_PKEY_CTX_free(pctx);

    return priv;
}

bytes serializePublic(const key *key)
{
    int len = i2d_PublicKey(key, nullptr);
    bytes buf(len);

    unsigned char *p = toUnsigned(buf);
    i2d_PublicKey(key, &p);

    return buf;
}

key *deserializePublic(bytes &data)
{
    const unsigned char *p = toUnsigned(data);
    key *k;
    return d2i_PublicKey(EVP_PKEY_ED25519, &k, &p, data.size());
}

bytes sign(key *k, bytes &data)
{
    size_t siglen = SIGSIZE;
    unsigned char *buf = static_cast<unsigned char *>(OPENSSL_zalloc(siglen));
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    bytes signature;

    // Load and sign data
    EVP_DigestSignInit(ctx, NULL, NULL, NULL, k);
    EVP_DigestSign(ctx, buf, &siglen, toUnsigned(data), data.size());

    signature = toBytes(buf, siglen);
    EVP_MD_CTX_free(ctx);
    OPENSSL_free(buf);
    return signature;
}

bool verify(key *k, bytes &data, bytes &signature)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    int ret;

    // Initialize verification context
    EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, k);
    ret = EVP_DigestVerify(ctx, toUnsigned(signature), SIGSIZE, toUnsigned(data), data.size());
    EVP_MD_CTX_free(ctx);

    return ret == 1;
}
