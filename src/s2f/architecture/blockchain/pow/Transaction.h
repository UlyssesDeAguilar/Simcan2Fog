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
        int amount;                          //<! Number to pay in Satoshis
        std::vector<std::byte> pubkeyScript; //<! Locking code

        /**
         * Computes the size of this output.
         *
         * @return output size.
         */
        size_t size() const
        {
            return sizeof(amount) + pubkeyScript.size();
        }

        /**
         * @return byte array representation of this object.
         */
        std::vector<std::byte> repr() const;
    };

    /**
     * Data representation of a transaction input
     */
    struct txi
    {
        sha256digest txid;                      //<! Transaction identifier
        int vout;                               //<! Output index
        std::vector<std::byte> signatureScript; //!< Unlocking code
        int sequenceNumber;                     //<! Minimum Mining Time / Replaceable

        /**
         * Computes the size of this input.
         *
         * @return input size.
         *
         */
        size_t size() const
        {
            return sizeof(txid) + sizeof(vout) + sizeof(sequenceNumber) + signatureScript.size();
        }

        /**
         * @return byte array representation of this object.
         */
        std::vector<std::byte> repr() const;
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
         * Computes the txid of this transaction.
         *
         * @return sha256 digest of the transaction.
         */
        sha256digest hash();

        /**
         * Computes the size in bytes of this transaction.
         *
         * @return transaction size.
         */
        size_t size() const;
    };
}

#endif
