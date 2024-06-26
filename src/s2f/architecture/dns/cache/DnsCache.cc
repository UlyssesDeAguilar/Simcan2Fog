#include "../../net/dns/DnsCache.h"
using namespace dns;
Define_Module(DnsCache);

void DnsCache::finish()
{
    ipCache.clear();
    tldCache.clear();
    nsCache.clear();
}

void DnsCache::insertData(const ResourceRecord &record)
{
    if (record.type == A)
    {
        ipCache[record.domain] = record;
    }
    else
    {
        cStringTokenizer tokenizer(record.domain.c_str(), ".");
        auto vec = tokenizer.asVector();

        if (vec.size() == 1)
        {
            tldCache[record.domain] = record;
        }
        else if (vec.size() == 2)
        {
            nsCache[record.domain] = record;
        }
        else
            error("While caching could not chache record into ns/tld");
    }
}