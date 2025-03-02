#include "s2f/management/cloudprovider/NodeDb.h"

using namespace omnetpp;

Define_Module(NodeDb);

void NodeDb::initialize()
{
    WATCH(table);
}

void NodeDb::addOrUpdateNode(const char *topic, const char *zone, uint64_t availableCores, uint64_t availableRam, uint64_t availableDisk)
{
    int index = table.addOrUpdateNode(topic, zone, availableCores, availableRam, availableDisk);
    EV_INFO << "Add/Update node with topic: " << topic << " - zone: " << zone << " - resources: " << getNode(index) << "\n";
}

int NodeDb::findNodeByTopic(const char *topic)
{
    return table.findNodeByTopic(topic);
}

const std::set<int> *NodeDb::findNodesByZone(const char *zone)
{
    return table.findNodesByZone(zone);
}
