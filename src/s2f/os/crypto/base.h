#ifndef __SIMCAN_CRYPTO_H__
#define __SIMCAN_CRYPTO_H__

#include <array>
#include <cstring>
#include <memory>
#include <openssl/x509.h>
#include <vector>

namespace s2f::os::crypto
{
    /* Arbitrary byte array */
    using bytes = std::vector<std::byte>;

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

    inline unsigned char *toUnsigned(bytes &buf)
    {
        return reinterpret_cast<unsigned char *>(buf.data());
    }
}
#endif
