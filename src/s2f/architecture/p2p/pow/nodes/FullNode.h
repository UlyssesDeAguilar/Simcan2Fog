#ifndef __POW_FULLNODE_H__
#define __POW_FULLNODE_H__

#include "../chain/Block.h"
#include "../chain/TxComparator.h"
#include "../chain/UtxoSet.h"
#include "omnetpp/csimplemodule.h"
#include <cstdint>
#include <omnetpp.h>
#include <queue>
#include <vector>

namespace s2f::p2p::pow
{

    class FullNode : public omnetpp::cSimpleModule
    {
      public:
        std::vector<Block> blockchain; //<! The chain.
        UtxoSet utxo;                  //!< Unspent funds in the chain.
        key priv;                      //<! Node's key for fee payout.
        bytes pubDer;                  //!< Serialization of the key (to compute only once).

        /* Mempool sorted by incoming transaction fee */
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
         *
         * TODO: Change hard-coded fakeness
         * TODO: Add 10-minute-average execution time.
         *
         * @return The mined block.
         */
        Block mineBlock();

        /**
         * Add a block to the chain after validation.
         *
         * BUG: vulnerable against double spending, since utxo is not updated
         * as transactions are validated.
         *
         * @param b Block received from a peer (or self).
         */
        void addBlock(const Block b);

      private:
        /**
         * Return the last node in the chain, if it exists.
         */
        const Block *last() const { return blockchain.empty() ? nullptr : &blockchain.back(); }

        /**
         * Creates a coinbase transaction and adds it to the current block.
         *
         * Since the coinbase transaction is chronologically added last,
         * this method also internally computes and sets the blcok merkle root.
         *
         * TODO: SeqNum / Witness.
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
         * TODO: BIP125 compatibility
         *
         * @param t transaction object.
         */
        void addToMempool(Transaction t);

        /**
         * Validate a tranasction against the node's utxo set.
         *
         * BUG: vulnerable against double spending, since utxo is not updated as
         * the inputs/outputs are validated.
         *
         * @param t Transaction to validate.
         *
         * @return True if the transaction is valid, false otherwise.
         */
        int validateTransaction(const Transaction &tx) const;
    };
}
#endif
