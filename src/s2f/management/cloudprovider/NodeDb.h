#ifndef SIMCAN_EX_NODEDB_H_
#define SIMCAN_EX_NODEDB_H_

#include "s2f/management/cloudprovider/NodeTable.h"
#include <omnetpp.h>

class NodeDb : public omnetpp::cSimpleModule
{
    protected:
        NodeTable table;

        virtual void initialize() override;

        virtual void finish() override { table.clear(); }

        virtual void handleMessage(omnetpp::cMessage *msg) override { error("This module doesn't take any messages"); }
    public:
        void addOrUpdateNode(const char *topic, const char *zone, uint64_t availableCores, uint64_t availableRam, uint64_t availableDisk);
        
        int findNodeByTopic(const char *topic);
        
        const std::set<int> *findNodesByZone(const char *zone);

        NodeEntry &getNode(int index) { return table.getNode(index); }
        const char* getTopicForNode(const NodeEntry &node) { return table.getTopicForNode(node); }
        const char* getZoneForNode(const NodeEntry &node) { return table.getZoneForNode(node); }
};

#endif /* SIMCAN_EX_NODEDB_H_ */