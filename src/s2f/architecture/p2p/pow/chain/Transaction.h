#ifndef __POW_TRANSACTION_H__
#define __POW_TRANSACTION_H__

#include "InOut.h"
#include <cstddef>
#include <cstdint>
#include <omnetpp.h>

using namespace s2f::os::crypto;
namespace s2f::p2p::pow
{

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

        Transaction();
        /**
         * Creates a transaction from received JSON data.
         *
         * @param data  NOTE: temporary.
         *
         * @return Transaction instance.
         */
        Transaction fromJSON(std::string data);

        /**
         * Add an output to this transaction.
         *
         * @param o Output.
         */
        void addOutput(struct utxo o) { outputs.push_back(o); }

        /**
         * Add an input to this transaction.
         *
         * @param i Input.
         */
        void addInput(struct txi i) { inputs.push_back(i); }

        /**
         * Build and add an output to a transaction.
         *
         * @param amount    Amount to pay.
         * @param pubDer    Address to pay to.
         */
        void addOutput(uint64_t amount, bytes &pubDer);

        /**
         * Build and add an input to a transaction.
         *
         * @param txid      Transaction where the funds are.
         * @param vout      Txid outpoint offset.
         * @param priv      Key that signs the amount, proving ownership.
         * @param pubDer    Ownership indicator at txid:vout.
         */
        void addInput(sha256digest txid, uint32_t vout, uint64_t amount, key &priv, const bytes &pubDer);

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
