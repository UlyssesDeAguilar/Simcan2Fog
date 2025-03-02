#include "s2f/management/cloudprovider/NodeTable.h"
#include <algorithm>

using namespace omnetpp;

int NodeTable::addOrUpdateNode(const char *topic, const char *zone, uint64_t availableCores, uint64_t availableRam, uint64_t availableDisk)
{
    ASSERT2(topic != nullptr, "Error, topic cannot be null");
    ASSERT2(zone != nullptr, "Error, zone cannot be null");

    auto iter = std::lower_bound(topics.begin(), topics.end(), topic);
    if (iter != topics.end() && !(topic < *iter))
    {
        NodeEntry &entry = nodes.at(iter->nodeId);
        entry.availableCores = availableCores;
        entry.availableRam = availableRam;
        entry.availableDisk = availableDisk;
        return iter->nodeId;
    }
    else
    {
        // Insert the new node
        int nodeIndex = nodes.size();
        nodes.emplace_back();
        NodeEntry &newNode = nodes.back();
        newNode.availableCores = availableCores;
        newNode.availableRam = availableRam;
        newNode.availableDisk = availableDisk;

        // Insert the new topic + sort
        int topicIndex = topics.size();
        topics.emplace_back();
        NodeTopic &newTopic = topics.back();
        newTopic.topic = topic;
        newTopic.nodeId = nodeIndex;
        std::sort(topics.begin(), topics.end());

        // Update the pointer for the node
        newNode.topicIndex = topicIndex;

        // Try to find the zone, if not add it
        auto zoneIter = std::lower_bound(zones.begin(), zones.end(), zone);
        if (zoneIter != zones.end() && !(zone < *zoneIter))
        {
            int zoneIndex = std::distance(zones.begin(), zoneIter);
            zoneIter->nodes.insert(nodeIndex);
            newNode.zoneIndex = zoneIndex;
        }
        else
        {
            int zoneIndex = zones.size();
            newNode.zoneIndex = zoneIndex;

            zones.emplace_back();
            NodeZone &newZone = zones.back();
            newZone.zone = zone;
            newZone.nodes.insert(nodeIndex);
            std::sort(zones.begin(), zones.end());
        }

        return newTopic.nodeId;
    }
}

int NodeTable::findNodeByTopic(const char *topic)
{
    auto iter = std::lower_bound(topics.begin(), topics.end(), topic);
    if (iter != topics.end() && !(topic < *iter))
        return iter->nodeId;
    else
        return -1;
}

const std::set<int> *NodeTable::findNodesByZone(const char *zone)
{
    auto iter = std::lower_bound(zones.begin(), zones.end(), zone);
    if (iter != zones.end() && !(zone < *iter))
        return &iter->nodes;
    else
        return nullptr;
}

std::ostream & operator<<(std::ostream &os, const NodeTable &table)
{
    os << "NodeTable - zones: " << table.zones.size() << " - topics/nodes: " << table.topics.size() << "\n";
    for (const auto &zone : table.zones)
    {
        os << "Zone: " << zone.zone;
        for (const auto &node : zone.nodes)
            os << node;
    }

    os << "\nTopics:\n";
    for (const auto &topic : table.topics)
        os << topic.topic << " : " << topic.nodeId << " -> " << table.nodes[topic.nodeId] << "\n";
    return os;
}

const char* NodeTable::getTopicForNode(const NodeEntry &node)
{
    return topics[node.topicIndex].topic.c_str();
}

const char* NodeTable::getZoneForNode(const NodeEntry &node)
{
    return zones[node.zoneIndex].zone.c_str();
}

std::ostream & operator<<(std::ostream &os, const NodeEntry &entry)
{
    os << "Cores: " << entry.availableCores << " - Ram: " << entry.availableRam << " - Disk: " << entry.availableDisk;
    return os;
}

bool operator<(const NodeTopic &a, const NodeTopic &b) { return strcmp(a.topic.c_str(), b.topic.c_str()) < 0; }
bool operator<(const NodeTopic &a, const char* topic) { return strcmp(a.topic.c_str(), topic) < 0; }
bool operator<(const char* topic, const NodeTopic &a) { return strcmp(topic, a.topic.c_str()) < 0; }

bool operator<(const NodeZone &a, const NodeZone &b) { return strcmp(a.zone.c_str(), b.zone.c_str()) < 0; }
bool operator<(const NodeZone &a, const char* zone) { return strcmp(a.zone.c_str(), zone) < 0; }
bool operator<(const char* zone, const NodeZone &a) { return strcmp(zone, a.zone.c_str()) < 0; }