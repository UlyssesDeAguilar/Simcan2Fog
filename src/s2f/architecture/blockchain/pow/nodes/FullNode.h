#ifndef __POW_FULLNODE_H__
#define __POW_FULLNODE_H__

#include "../data/Block.h"
#include "omnetpp/csimplemodule.h"
#include "s2f/architecture/blockchain/pow/database/UtxoSet.h"
#include <omnetpp.h>
#include <vector>

namespace s2f::chain::pow
{
    class FullNode : public omnetpp::cSimpleModule
    {
      public:
        std::vector<Block> blockchain;
        UtxoSet utxo;

        virtual void initialize(int stage) override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
    };
}
#endif
