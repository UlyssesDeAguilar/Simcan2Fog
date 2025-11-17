#ifndef __POW_TRANSACTION_H__
#define __POW_TRANSACTION_H__

#include "s2f/os/crypto/crypto.h"
#include <cstddef>
#include <omnetpp.h>

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
    };

    /**
     * @class Transaction Transaction.h "Transaction.h"
     *
     * Data representation of a transaction within bitcoin.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-11-07
     */
    class Transaction
    {
      public:
        int version;                      //!< Version number
        std::vector<struct utxo> outputs; //!< Payouts for transaction
        std::vector<struct txi> inputs;   //!< Funds for transaction
        int locktime;                     //!< Earliest time the transaction can be added to the blockchain

        /**
         * Creates a transaction from received JSON data.
         *
         * @param data  NOTE: temporary.
         *
         * @return Transaction instance.
         */
        Transaction fromJSON(std::string data);

        /**
         * Computes the txid of this transaction from transaction data.
         *
         * @return sha256 digest of the transaction.
         */
        sha256digest hash() const;

        /**
         * Returns the size of this transaction in bytes.
         *
         * @return transaction size.
         */
        size_t size() const;
    };
}

#endif
