#include "s2f/architecture/dns/db/DnsDb.h"

using namespace inet;
using namespace s2f::dns;

Define_Module(DnsDb);

void DnsDb::initialize()
{
    cValueArray *zoneConfig = check_and_cast<cValueArray *>(par("zoneConfig").objectValue());
    cEnum *recordTypeEnum = cEnum::find("RRType", "s2f::dns");

    EV_DEBUG << "Loading zone configuration\n";
    for (int i = 0; i < zoneConfig->size(); i++)
    {
        auto zoneMap = check_and_cast<cValueMap *>(zoneConfig->get(i).objectValue());

        // Get zone
        const char *zone = zoneMap->get("zone").stringValue();
        cValueArray *records = check_and_cast<cValueArray *>(zoneMap->get("records").objectValue());

        // Extract and insert records
        EV_DEBUG << "Inserting records for zone " << zone << "\n";
        for (int j = 0; j < records->size(); j++)
        {
            auto recordMap = check_and_cast<cValueMap *>(records->get(j).objectValue());
            readAndInsertRecord(zone, recordMap, recordTypeEnum);
        }
    }

    EV_DEBUG << "DnsDb initialized\n";
    EV_DEBUG << tree;
    WATCH(tree);
}

void DnsDb::readAndInsertRecord(const char *zone, omnetpp::cValueMap *recordMap, cEnum *recordTypeEnum)
{
    std::string domain = recordMap->get("domain").stdstringValue();
    RRType type = static_cast<RRType>(recordTypeEnum->resolve(recordMap->get("type").stringValue()));
    uint32_t ttl = recordMap->get("ttl").intValue();

    ResourceRecord record = buildRecord(domain, type, ttl, recordMap);

    EV_DEBUG << "Inserting record " << record << "\n";
    tree.insertRecord(zone, &record);
}

ResourceRecord DnsDb::buildRecord(std::string &name, RRType type, uint32_t ttl, omnetpp::cValueMap *recordMap)
{
    recordMap->get("text").stringValue();
    L3Address(recordMap->get("ip").stringValue());

    if (type == RRType::A || type == RRType::AAAA)
    {
        return ResourceRecord(name, L3Address(recordMap->get("ip").stringValue()), ttl);
    }
    else if (type == RRType::CNAME)
    {
        return ResourceRecord(name, recordMap->get("cname").stdstringValue(), ttl);
    }
    else if (type == RRType::TXT)
    {
        return ResourceRecord(name, recordMap->get("text").stdstringValue(), ttl);
    }
    else{
        error("Unsupported record type");
    return ResourceRecord();
    }
}
void DnsDb::insertRecord(const char *zone, const ResourceRecord &record)
{
    EV_INFO << "Inserting record for zone " << zone << " : " << record << "\n";
    tree.insertRecord(zone, &record);
}

void DnsDb::removeRecord(const char *zone, const ResourceRecord &record)
{
    EV_INFO << "Deleting record for zone " << zone << " : " << record << "\n";
    tree.removeRecord(zone, &record);
}

const DnsTreeNode *DnsDb::searchRecords(const DnsQuestion &question)
{
    EV_DEBUG << "Searching records for domain " << question.getDomain() << "\n";
    return tree.searchRecords(question.getDomain());
}
