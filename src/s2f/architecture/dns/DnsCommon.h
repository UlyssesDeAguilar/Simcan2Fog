/**
 * @file DnsCommon.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines and concentrates common concepts of the DNS protocol
 * @version 1.0
 * @date 2025-02-02
 */

#ifndef SIMCAN_EX_DNS_COMMON_H__
#define SIMCAN_EX_DNS_COMMON_H__

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

    /**
     * @brief Parses the domain given a level of the DNS tree
     * in order to generate the appropiate DNS level tree
     *
     * @param domain Domain to parse
     * @param level The DNS level
     * @return omnetpp::opp_string The parsed domain for the question
     */
    omnetpp::opp_string getQuestionForLevel(const char *domain, DnsLevel level);

    /**
     * @brief Attempts to figure out if a record falls under the authority domain
     * 
     * @param record Record to be matched
     * @param domain Domain
     * @return bool If the record falls under the domain authority 
     */
    bool matchesAuthorityDomain(const ResourceRecord &record, const char *domain);

    /**
     * @brief Attempts to match a record with a domain, possibly a wildcard domain
     * 
     * @param record The Resource Record to be matched 
     * @param domain The domain
     * @return bool If the record matches the domain
     */
    bool matchWithWildcard(const ResourceRecord &record, const char *domain);

    /**
     * @brief Matches a record with a question type
     * 
     * @param record The resource record type
     * @param query The query record type
     * @return bool If the record matches the query type
     */
    static bool matchesQuestionType(RecordType record, RecordType query) { return record == query || query == RecordType::ANY; }

    /**
     * @brief Translates a string into a RecordType
     *
     * @param type The record type in string format
     * @return RecordType The record type
     */
    RecordType getRecordType(const char *type);
}

#endif /* SIMCAN_EX_DNS_COMMON_H__ */
