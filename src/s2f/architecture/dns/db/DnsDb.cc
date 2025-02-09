#include "s2f/architecture/dns/db/DnsDb.h"

using namespace omnetpp;
using namespace dns;

Define_Module(DnsDb);

void DnsDb::insertRecord(const char *domain, const ResourceRecord *record)
{
    EV_INFO << "Inserting records for domain " << domain << "\n";
    ResourceRecord *recordCopy = new ResourceRecord(*record);
    tree.insertRecord(domain, recordCopy);
}

void DnsDb::removeRecord(const char *domain, const ResourceRecord *record)
{
    EV_INFO << "Deleting records for domain " << domain << "\n";
    tree.removeRecord(domain, record);
}

const std::vector<ResourceRecord *> *DnsDb::searchRecords(const char *domain)
{
    EV_DEBUG << "Searching records for domain " << domain << "\n";
    return tree.searchRecords(domain);
}