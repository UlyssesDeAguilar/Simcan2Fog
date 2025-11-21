#ifndef __POW_TX_INDEX_H__
#define __POW_TX_INDEX_H__

#include "../data/Transaction.h"
#include "s2f/os/crypto/crypto.h"
#include <omnetpp.h>

namespace s2f::chain::pow
{

    /**
     * @class UtxoSet UtxoSet.h "UtxoSet.h"
     *
     * Per-node database containing unspent transaction outputs.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-11-08
     */
    class UtxoSet
    {
      public:
        std::map<sha256digest, std::vector<struct utxo>> database;

        /**
         * Adds outputs from a transaction to the unspent set.
         *
         * @param t Original transaction.
         */
        void add(Transaction t) { database[t.hash()] = t.outputs; }

        /**
         * Retrieves the coin amount remaining at txid:vout.
         *
         * @param txid Transaction identifier (hash).
         * @param vout Transaction outpoint index.
         * @return The amount in the outpoint or -1.
         */
        uint64_t getCoin(const sha256digest &txid, int vout) const;

        /**
         * Invalidates the coin value at txid:vout so that it cannot be spent
         * via getCoin in the future.
         *
         * @param txid Transaction identifier (hash).
         * @param vout Transaction outpoint index.
         */
        void spendCoin(const sha256digest &txid, int vout);
    };
}
#endif
