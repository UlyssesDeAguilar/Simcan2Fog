#ifndef __POW_FULLNODE_H__
#define __POW_FULLNODE_H__

#include "../data/Block.h"
#include "../data/TxComparator.h"
#include "../data/UtxoSet.h"
#include "omnetpp/csimplemodule.h"
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

        TxComparator txc;
        std::priority_queue<TxFee, std::vector<TxFee>, TxComparator> mempool{txc};

        virtual void initialize(int stage) override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;

        /**
         * Starts mining a block using transactions from the mempool.
         * TODO: replace `limit` with the actual block limit
         *
         * @param limit Maximum number of transactions added to the block.
         *
         */
        void mineBlock(size_t limit);

      private:
        /**
         * Creates a coinbase transaction and adds it to the current block.
         */
        void addCoinbase();

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
         * Adds a list of transactions to the mempool of this node.
         *
         * This method only only accepts transactions that are valid for the
         * local block chain, failing silently otherwise.
         *
         * @param trns  List of transactions to insert.
         */
        void addToMempool(std::vector<Transaction> &trns);
    };
}
#endif
