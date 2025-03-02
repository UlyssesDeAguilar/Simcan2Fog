#ifndef SIMCAN_EX_NODETABLE_H_
#define SIMCAN_EX_NODETABLE_H_

#include <omnetpp.h>

struct NodeEntry
{
    uint64_t availableCores;
    uint64_t availableRam;
    uint64_t availableDisk;
    int topicIndex;
    int zoneIndex;
    friend std::ostream &operator<<(std::ostream &os, const NodeEntry &entry);
};

struct NodeTopic
{
    omnetpp::opp_string topic;
    int nodeId;
};

bool operator<(const NodeTopic &a, const NodeTopic &b);
bool operator<(const NodeTopic &a, const char *topic);
bool operator<(const char *topic, const NodeTopic &a);

struct NodeZone
{
    omnetpp::opp_string zone;
    std::set<int> nodes;
};

bool operator<(const NodeZone &a, const NodeZone &b);
bool operator<(const NodeZone &a, const char *zone);
bool operator<(const char *zone, const NodeZone &a);

class NodeTable
{
    std::vector<NodeEntry> nodes;
    std::vector<NodeTopic> topics;
    std::vector<NodeZone> zones;

public:
    int findNodeByTopic(const char *topic);

    const std::set<int> *findNodesByZone(const char *zone);

    NodeEntry &getNode(int index) { return nodes.at(index); }

    const char *getTopicForNode(const NodeEntry &node);

    const char *getZoneForNode(const NodeEntry &node);

    int addOrUpdateNode(const char *topic, const char *zone, uint64_t availableCores, uint64_t availableRam, uint64_t availableDisk);

    friend std::ostream &operator<<(std::ostream &os, const NodeTable &table);

    void clear()
    {
        nodes.clear();
        topics.clear();
        zones.clear();
    }
};

#endif /* SIMCAN_EX_NODETABLE_H_ */