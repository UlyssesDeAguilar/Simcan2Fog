#include "s2f/architecture/dns/db/DnsDb.h"

using namespace inet;
using namespace s2f::dns;

Define_Module(DnsDb);

void DnsDb::initialize()
{
    cValueArray *zoneConfig = check_and_cast<cValueArray *>(par("zoneConfig").objectValue());

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
            const char *text = nullptr;
            L3Address ip;

            auto recordMap = check_and_cast<cValueMap *>(records->get(j).objectValue());
            const char *domain = recordMap->get("domain").stringValue();
            RecordType type = getRecordType(recordMap->get("type").stringValue());

            if (type == RecordType::CNAME || type == RecordType::TXT)
                text = recordMap->get("text").stringValue();
            else
                ip = L3Address(recordMap->get("ip").stringValue());

            ResourceRecord record;
            record.domain = domain;
            record.type = type;
            record.ip = ip;
            record.contents = text;

            EV_DEBUG << "Inserting record " << record << "\n";
            tree.insertRecord(zone, &record);
            WATCH(tree);
        }
    }

    EV_DEBUG << "DnsDb initialized\n";
    EV_DEBUG << tree;
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
