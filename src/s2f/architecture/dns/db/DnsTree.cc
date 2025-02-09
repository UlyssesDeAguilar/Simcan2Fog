#include "s2f/architecture/dns/db/DnsTree.h"
#include <algorithm>

using namespace dns;
using namespace omnetpp;

DnsTreeNode::~DnsTreeNode()
{
    for (auto &child : children)
        delete child.second;

    children.clear();

    for (auto &record : records)
        delete record;

    records.clear();
}

DnsTreeNode *DnsTreeNode::getChild(const char *domain)
{
    auto iter = children.find(domain);
    if (iter == children.end())
        return nullptr;
    return iter->second;
}

DnsTreeNode *DnsTreeNode::getChildOrCreate(const char *domain)
{
    auto iter = children.find(domain);
    if (iter != children.end())
        return iter->second;

    DnsTreeNode *node = new DnsTreeNode(domain);
    children[domain] = node;
    return node;
}

std::vector<ResourceRecord *> *DnsTree::searchRecords(const char *domain)
{
    // Tokenize the FQDN
    cStringTokenizer tokenizer(domain, ".");
    auto subdomains = tokenizer.asVector();

    // Start from the root
    DnsTreeNode *node = root->getChild(subdomains.back().c_str());
    subdomains.pop_back();

    while (subdomains.size() > 0 && node != nullptr)
    {
        auto &current = subdomains.back();

        // If the node is in the last subdomain we finished
        // If no more children, maybe we have a NS who knows
        if (subdomains.size() == 1 || node->getChildCount() == 0)
            return node->getRecords();

        // Continue the search down the tree
        node = node->getChild(subdomains.back().c_str());

        subdomains.pop_back();
    }

    return nullptr;
}

void DnsTree::insertRecord(const char *domain, ResourceRecord *record)
{
    // Tokenize the FQDN
    cStringTokenizer tokenizer(domain, ".");
    auto subdomains = tokenizer.asVector();
    std::string current = std::move(subdomains.back());
    subdomains.pop_back();

    // Start from the root
    DnsTreeNode *node = root->getChildOrCreate(current.c_str());

    while (subdomains.size() > 0)
    {
        current = std::move(subdomains.back());
        subdomains.pop_back();

        if (subdomains.size() == 1)
            node->addRecord(record);
        else
            node = node->getChildOrCreate(current.c_str());
    }
}

bool DnsTree::removeRecord(const char *domain, const ResourceRecord *record)
{
    std::vector<ResourceRecord *> *records = searchRecords(domain);

    if (records == nullptr)
        return false;

    auto iter = std::find_if(records->begin(), records->end(), [record](ResourceRecord *rr)
                             { return rr->getDomain() == record->getDomain() && rr->getType() == record->getType(); });

    if (iter == records->end())
        return false;

    // Swap the records, delete and then pop back!
    std::swap(*iter, *(records->end() - 1));
    delete *iter;
    records->pop_back();

    return true;
}

void DnsTree::clear()
{
    delete root;
    root = new DnsTreeNode(".");
}

void DnsTreeNode::print(std::ostream &out, int level) const
{
    if (level > 3)
    {
        std::string indent(level - 3, ' ');
        out << indent;
    }
    
    out << "|->" << domain << "\n";

    for (auto &child : children)
        child.second->print(out, level + 3);
}