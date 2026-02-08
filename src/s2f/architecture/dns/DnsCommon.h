/**
 * @file DnsCommon.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines and concentrates common concepts of the DNS protocol
 * @details Based on the RFC 1034/1035 and the explanation found in:
 *      http://www.tcpipguide.com/free/t_DNSNameServerDataStorageResourceRecordsandClasses-3.htm#Table_166
 * @version 2.0
 * @date 2026-01-17
 */

#ifndef SIMCAN_EX_DNS_COMMON_H__
#define SIMCAN_EX_DNS_COMMON_H__

#include "inet/networklayer/common/L3Address.h"
#include "s2f/architecture/dns/DnsRequest_m.h"
#include "s2f/architecture/dns/ResourceRecord_m.h"
#include <memory>
#include <omnetpp.h>

namespace s2f::dns
{
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
}

#endif /* SIMCAN_EX_DNS_COMMON_H__ */
