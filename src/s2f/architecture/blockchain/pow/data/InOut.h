#ifndef __POW_INOUT_H__
#define __POW_INOUT_H__

#include "s2f/os/crypto/crypto.h"
#include <cstddef>
#include <omnetpp.h>

using namespace s2f::os::crypto;
namespace s2f::chain::pow
{

    /**
     * Data representation of a transaction output
     */
    struct utxo
    {
        uint64_t amount;    //<! Number to pay in Satoshis
        bytes pubkeyScript; //<! Locking code

        /**
         * Computes the size of this output.
         *
         * @return output size.
         */
        size_t size() const { return sizeof(amount) + pubkeyScript.size(); }

        /**
         * @return byte array representation of this object.
         */
        bytes getBytes() const;

        /**
         * Wrapper to decapsulate a pubkey script.
         *
         * @param[out] pubHash  Public key hash.
         */
        void getPubkeyScript(ripemd160digest &pubHash) const;

        void buildPubkeyScript(bytes &pubDer);
    };

    /**
     * Data representation of a transaction input
     */
    struct txi
    {
        sha256digest txid;       //<! Transaction identifier
        uint32_t vout;           //<! Output index
        bytes signatureScript;   //!< Unlocking code
        uint32_t sequenceNumber; //<! Minimum Mining Time / Replaceable

        /**
         * Computes the size of this input.
         *
         * @return input size.
         */
        size_t size() const { return sizeof(txid) + sizeof(vout) + sizeof(sequenceNumber) + signatureScript.size(); }

        /**
         * @return byte array representation of this object.
         */
        bytes getBytes() const;

        /**
         * Wrapper that creates a signature script to access inputs.
         *
         * @param amount    Satoshis in the input.
         * @param priv      Private key used to sign the amount.
         * @param pubDer    Serialized public key to verify the signature.
         *
         */
        void buildSignatureScript(uint64_t amount, const key &priv, const bytes &pubDer);

        /**
         * Wrapper to decapsulate a signature script.
         *
         * @param[out] signature    Signed data.
         * @param[out] pubDer       Serialized public key.
         * @param[out] pub          Openssl format public key. De-serialized from
         *                          from pubDer on-the-fly.
         */
        void getSignatureScript(bytes &signature, bytes &pubDer, key &pub) const;
    };
}

#endif
