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

        void mineBlock();

      private:
        void addCoinbase();
    };
}
#endif
