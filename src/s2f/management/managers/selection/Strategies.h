#ifndef SIMCAN_EX_SELECTION_STRATEGIES_H_
#define SIMCAN_EX_SELECTION_STRATEGIES_H_

#include <omnetpp.h>
#include "s2f/management/managers/Node.h"
#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"

namespace dc
{
    class SelectionStrategy : public omnetpp::cObject
    {
    public:
        virtual ~SelectionStrategy() {}
        virtual bool selectNode(const VirtualMachine *vmSpecs, const std::vector<Node> &nodes, size_t &index) = 0;
    };

    class FirstFit : public SelectionStrategy
    {
    public:
        FirstFit() : SelectionStrategy() {}
        virtual bool selectNode(const VirtualMachine *vmSpecs, const std::vector<Node> &nodes, size_t &index);
    };

    class BestFit : public SelectionStrategy
    {
    public:
        BestFit() : SelectionStrategy() {}
        virtual bool selectNode(const VirtualMachine *vmSpecs, const std::vector<Node> &nodes, size_t &index);
    };
}

#endif /* SIMCAN_EX_DC_SELECTION_STRATEGIES */
