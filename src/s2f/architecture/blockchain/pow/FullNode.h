#ifndef __POW_FULLNODE_H__
#define __POW_FULLNODE_H__

#include "Block.h"
#include "omnetpp/csimplemodule.h"
#include <omnetpp.h>
#include <vector>

namespace s2f::chain::pow
{
    class FullNode : public omnetpp::cSimpleModule
    {
      public:
        std::vector<Block> blockchain;

        virtual void initialize(int stage) override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
    };
}
#endif
