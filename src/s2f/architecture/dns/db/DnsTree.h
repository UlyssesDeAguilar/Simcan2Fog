#ifndef SIMCAN_EX_DNS_TREE
#define SIMCAN_EX_DNS_TREE

#include "s2f/architecture/dns/DnsCommon.h"

namespace dns
{
    constexpr int NUM_DNS_LEVELS = 3;

    static const char *dnsLevelToString(DnsLevel level)
    {
        static const char *levels[] = {"ROOT", "TLD", "AUTHORITATIVE"};
        return levels[level];
    }

    class DnsTreeNode
    {
    protected:
        omnetpp::opp_string domain;
        DnsTreeNode *parent;
        DnsLevel level;
        std::map<omnetpp::opp_string, DnsTreeNode> children;
        std::set<ResourceRecord> records;

    public:
        DnsTreeNode(const char *domain, DnsLevel level, DnsTreeNode *parent) : domain(domain), parent(parent), level(level) {}

        DnsTreeNode() : DnsTreeNode("", ROOT, nullptr) {}

        virtual ~DnsTreeNode() = default;

        void insertElement(DnsTreeNode *node) { children[node->getDomain()] = *node; }

        DnsTreeNode *getChild(const char *domain);

        /**
         * @brief Get the corresponding child node, or creates a new child
         * @param domain Subdomain of the child
         * @return DnsTreeNode& The created node
         */
        DnsTreeNode *getChildOrCreate(const char *domain);

        int getChildCount() const { return children.size(); }

        const char *getDomain() const { return domain.c_str(); }

        DnsLevel getDnsLevel() const { return level; }

        const DnsTreeNode *getParent() const { return parent; }

        std::set<ResourceRecord> *getRecords() { return records.size() > 0 ? &records : nullptr; }

        const std::set<ResourceRecord> *getRecords() const { return records.size() > 0 ? &records : nullptr; }

        void addRecord(const ResourceRecord *record) { records.insert(*record); }

        friend std::ostream &operator<<(std::ostream &out, const DnsTreeNode &node);

        std::ostream &print(std::ostream &out, int level = 0) const;

        void clear();
    };

    class DnsTree
    {
    protected:
        DnsTreeNode root;
        DnsTreeNode *searchRecordsForUpdate(const char *domain);

    public:
        DnsTree();
        virtual ~DnsTree() = default;
        void insertRecord(const char *zone, const ResourceRecord *record);
        bool removeRecord(const char *zone, const ResourceRecord *record);
        const DnsTreeNode *searchRecords(const char *domain) { return static_cast<const DnsTreeNode *>(searchRecordsForUpdate(domain)); }
        void clear();
        friend std::ostream &operator<<(std::ostream &os, const DnsTree &tree) { return tree.root.print(os); }
    };
}

#endif // SIMCAN_EX_DNS_TREE