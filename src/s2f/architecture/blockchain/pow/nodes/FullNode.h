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
        void mineBlock();

      private:
        /**
         * Creates a coinbase transaction and adds it to the current block.
         *
         * Since the coinbase transaction is chronologically added last,
         * this method also internally computes and sets the blcok merkle root.
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
