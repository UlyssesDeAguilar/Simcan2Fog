#ifndef SIMCAN_EX_DNS_TREE
#define SIMCAN_EX_DNS_TREE

#include "s2f/architecture/dns/ResourceRecord_m.h"

namespace dns
{
    class DnsTreeNode
    {
    protected:
        omnetpp::opp_string domain;
        std::map<omnetpp::opp_string, DnsTreeNode *> children;
        std::vector<ResourceRecord *> records;

    public:
        DnsTreeNode(const char *domain) : domain(domain) {}
        virtual ~DnsTreeNode();

        void insertElement(DnsTreeNode *node) { children[node->getDomain()] = node; }
        DnsTreeNode *getChild(const char *domain);
        DnsTreeNode *getChildOrCreate(const char *domain);
        int getChildCount() const { return children.size(); }
        const char *getDomain() const { return domain.c_str(); }
        std::vector<ResourceRecord *> *getRecords() { return records.size() > 0 ? &records : nullptr; }
        void addRecord(ResourceRecord *record) { records.push_back(record); }
        void print(std::ostream &out, int level = 0) const;
    };

    class DnsTree
    {
    protected:
        DnsTreeNode *root;

    public:
        DnsTree() { root = new DnsTreeNode("."); }
        
        virtual ~DnsTree() { delete root; }

        void insertRecord(const char *domain, ResourceRecord *record);

        bool removeRecord(const char *domain, const ResourceRecord *record);

        std::vector<ResourceRecord *> *searchRecords(const char *domain);

        void clear();

        void print(std::ostream &out) const { root->print(out); }
    };
}

#endif // SIMCAN_EX_DNS_TREE