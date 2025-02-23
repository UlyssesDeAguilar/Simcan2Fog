/**
 * @file common.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines and concentrates common concepts of the DNS protocol
 * @version 1.0
 * @date 2025-02-02
 */

#ifndef SIMCAN_EX_DNS_COMMON_H
#define SIMCAN_EX_DNS_COMMON_H

#include <memory>
#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"
#include "s2f/architecture/dns/ResourceRecord_m.h"
#include "s2f/architecture/dns/DnsRequest_m.h"


namespace dns
{
    /*
        Based on the RFC 1034/1035 and the explanation found in:
        http://www.tcpipguide.com/free/t_DNSNameServerDataStorageResourceRecordsandClasses-3.htm#Table_166
    */
    constexpr uint32_t DNS_PORT = 53;                          //!< The standard DNS port
    static const inet::Ipv4Address ROOT_DNS_IP("199.7.83.42"); //!< Root DNS server IP (in reality is the I root server - ICANN)

    enum DnsLevel
    {
        ROOT,
        TLD,
        AUTHORITATIVE
    };

    static bool operator<(const ResourceRecord &lhs, const ResourceRecord &rhs)
    {
        return (omnetpp::opp_strcmp(lhs.domain.c_str(), rhs.domain.c_str()) < 0) || (lhs.type < rhs.type);
    }

    std::ostream &operator<<(std::ostream &os, const ResourceRecord &obj);

    std::ostream &operator<<(std::ostream &os, const RecordType type);

    omnetpp::opp_string getQuestionForLevel(const char *domain, DnsLevel level);

    bool matchesAuthorityDomain(const ResourceRecord &record, const char *domain);

    bool matchWithWildcard(const ResourceRecord &record, const char *domain);

    static bool matchesQuestionType(RecordType record, RecordType query) { return record == query || query == RecordType::ANY; }

    RecordType getRecordType(const char *type);
}

#endif
