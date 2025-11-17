#ifndef __POW_FULLNODE_H__
#define __POW_FULLNODE_H__

#include "../data/Block.h"
#include "omnetpp/csimplemodule.h"
#include "s2f/architecture/blockchain/pow/database/UtxoSet.h"
#include <omnetpp.h>
#include <queue>
#include <vector>

namespace s2f::chain::pow
{

    struct TxFee
    {
        Transaction t;
        uint64_t fee;
    };

    class TxComparator
    {
        UtxoSet &utxo;

      public:
        TxComparator(UtxoSet &utxoSet) : utxo(utxoSet) {}

        bool operator()(const TxFee &tx1, const TxFee &tx2)
        {
            return tx1.fee > tx2.fee;
        }
    };

    class FullNode : public omnetpp::cSimpleModule
    {
      public:
        std::vector<Block> blockchain;
        UtxoSet utxo;

        TxComparator txc{utxo};
        std::priority_queue<TxFee, std::vector<TxFee>, TxComparator> mempool{txc};

        virtual void initialize(int stage) override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;

        void mineBlock();
        void addCoinbase();
    };
}
#endif
