#ifndef __POW_FULLNODE_H__
#define __POW_FULLNODE_H__

#include "../data/Block.h"
#include "../data/TxComparator.h"
#include "../data/UtxoSet.h"
#include "omnetpp/csimplemodule.h"
#include <cstdint>
#include <omnetpp.h>
#include <queue>
#include <vector>

namespace s2f::chain::pow
{

    class FullNode : public omnetpp::cSimpleModule
    {
      public:
        std::vector<Block> blockchain;
        UtxoSet utxo;
        key priv;
        bytes pubDer;

        TxComparator txc;
        std::priority_queue<TxFee, std::vector<TxFee>, TxComparator> mempool{txc};

        /**
         * Initialization hook for this module.
         *
         * @param stage Initialization stage.
         */
        virtual void initialize(int stage) override;

        /**
         * Handler for received messages.
         */
        virtual void handleMessage(omnetpp::cMessage *msg) override;

        /**
         * Build and mine a block using transactions from the mempool.
         *
         * Respects the maximum size for a serialized block.
         */
        Block mineBlock();

        /**
         * Add a block to the chain after validation.
         *
         * @param b Block received from a peer (or self).
         */
        void addBlock(const Block b);

      private:
        const Block *last() const { return blockchain.empty() ? nullptr : &blockchain.back(); }
        /**
         * Creates a coinbase transaction and adds it to the current block.
         *
         * Since the coinbase transaction is chronologically added last,
         * this method also internally computes and sets the blcok merkle root.
         *
         * @param block The block that the transaction goes into.
         */
        void addCoinbase(Block &block);

        /**
         * Adds a transaction to the mempool of this node.
         *
         * This method only only accepts transactions that are valid for the
         * local block chain, failing silently otherwise.
         *
         * @param t transaction object.
         */
        void addToMempool(Transaction t);

        /**
         * Validate a tranasction against the node's utxo set.
         *
         * @param t Transaction to validate.
         *
         * @return True if the transaction is valid, false otherwise.
         */
        int validateTransaction(const Transaction &tx) const;

        //
        //
        //
        //
        //
        //
        //
        // Temporary test modules
        //
        //
        //
        //
        //
        //
        //

        /**
         * Calls the crypto api methods
         */
        void dummyCryptoApiTest();
        void dummyBlockCreationTest();
    };
}
#endif
