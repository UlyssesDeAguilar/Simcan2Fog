#include "s2f/architecture/dns/db/DnsTree.h"
#include <algorithm>

using namespace s2f::dns;
using namespace omnetpp;

void DnsTreeNode::clear()
{
    children.clear();
    records.clear();
}

DnsTreeNode *DnsTreeNode::getChild(const char *domain)
{
    auto iter = children.find(domain);
    if (iter == children.end())
        return nullptr;
    return &iter->second;
}

DnsTreeNode *DnsTreeNode::getChildOrCreate(const char *domain)
{
    auto iter = children.find(domain);
    if (iter != children.end())
        return &iter->second;

    children[domain] = DnsTreeNode(domain, (DnsLevel)(level + 1), this);
    return &children[domain];
}

DnsTreeNode *DnsTree::searchRecordsForUpdate(const char *domain)
{
    // Tokenize the FQDN
    cStringTokenizer tokenizer(domain, ".");
    auto subdomains = tokenizer.asVector();

    // Special case, if no tokens (or no children) it means "." so it goes for the root
    if (subdomains.size() == 0 || root.getChildCount() == 0)
        return &root;

    // Start from the root
    DnsTreeNode *node = root.getChild(subdomains.back().c_str());
    subdomains.pop_back();

    while (subdomains.size() > 0 && node != nullptr)
    {
        // If no more children, maybe we have a NS or a wildcard domain
        if (node->getChildCount() == 0)
            return node;

        // Continue the search down the tree
        node = node->getChild(subdomains.back().c_str());
        subdomains.pop_back();
    }

    return node;
}

std::ostream &DnsTreeNode::print(std::ostream &out, int level) const
{
    std::string indent;
    if (level > 0)
        indent.append(level * 3, ' ');

    out << indent << "|->" << domain << "[" << dnsLevelToString(this->level) << "]\n";

    for (auto &record : records)
        out << indent << " + " << record << "\n";

    for (auto &child : children)
        child.second.print(out, level + 1);
    return out;
}

namespace s2f
{
    namespace dns
    {
        std::ostream &operator<<(std::ostream &out, const DnsTreeNode &node)
        {
            out << "DnsTreeNode of zone: " << node.domain << " level:" << dnsLevelToString(node.level) << "\n";
            out << "Records: ";
            for (auto &record : node.records)
                out << " + " << record << "\n";
            out << "Children: ";
            for (auto &child : node.children)
                out << " + " << child.second.domain << "\n";
            return out;
        }
    }
}

DnsTree::DnsTree()
{
    root = DnsTreeNode();
}

void DnsTree::insertRecord(const char *zone, const ResourceRecord *record)
{
    // Tokenize the FQDN
    cStringTokenizer tokenizer(zone, ".");
    std::vector<std::string> subdomains = tokenizer.asVector();

    // Special case, if no tokens it means "." so it goes for the root
    if (subdomains.size() == 0)
    {
        root.addRecord(record);
        return;
    }

    // Start from the root
    DnsTreeNode *node = root.getChildOrCreate(subdomains.back().c_str());
    subdomains.pop_back();

    for (int i = 0; i < (NUM_DNS_LEVELS - 2) && subdomains.size() > 0; i++)
    {
        node = node->getChildOrCreate(subdomains.back().c_str());
        subdomains.pop_back();
    }

    node->addRecord(record);
}

bool DnsTree::removeRecord(const char *zone, const ResourceRecord *record)
{
    DnsTreeNode *node = searchRecordsForUpdate(zone);
    if (!node)
        return false;

    // Search the record and destroy it
    auto records = node->getRecords();
    return records->erase(*record) > 0;
}

void DnsTree::clear()
{
    root.clear();
}
