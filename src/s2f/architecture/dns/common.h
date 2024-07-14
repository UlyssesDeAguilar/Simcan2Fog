/**
 * @file common.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines and concentrates common concepts of the DNS protocol
 * @version 0.1
 * @date 2023-11-01
 *
 */

#ifndef SIMCAN_EX_DNS_COMMON
#define SIMCAN_EX_DNS_COMMON

#include <memory>
#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"

namespace dns
{
    using namespace omnetpp;
    using namespace inet;

    /*
        Based on the RFC 1034/1035 and the explanation found in:
        http://www.tcpipguide.com/free/t_DNSNameServerDataStorageResourceRecordsandClasses-3.htm#Table_166
    */

    constexpr uint32_t DNS_PORT = 53;

    // Root DNS server IP (in reality is the I root server - ICANN)
    static const Ipv4Address ROOT_DNS_IP("199.7.83.42");

    typedef enum
    {
        QUERY,  // Standard query
        IQUERY, // Used for inverse querys 				(obsolete)
        STATUS, // Request for status					(currently not used)
        NOTIFY  // Notify for Zone Transfer				(currently not used)
    } OP_Code;

    typedef enum
    {
        NOERROR,  // DNS Query completed successfully
        FORMERR,  // DNS Query Format Error
        SERVFAIL, // Server failed to complete the DNS request
        NXDOMAIN, // Domain name does not exist
        NOTIMP,   // Function not implemented
        REFUSED,  // The server refused to answer for the query (currently not used)
        YXDOMAIN, // Name that should not exist, does exist	  (currently not used)
        XRRSET,   // RRset that should not exist, does exist	  (currently not used)
        NOTAUTH,  // Server not authoritative for the zone	  (currently not used)
        NOTZONE   // Name not in zone							  (currently not used)
    } ReturnCode;

    struct ResourceRecord
    {
        opp_string domain;
        inet::L3Address ip;
        enum RR_Type
        {
            A,     // Address
            NS,    // Name Server
            CNAME, // Canonical Name
            SOA,   // Start of Authority
            PTR,   // Pointer
            MX,    // eMail eXchange
            TXT    // Text string (arbitrary text)
        } type;
        

        const char *typeToStr() const
        {
            static const char *RR_TypeToStr[] = {"A", "NS", "CNAME", "SOA", "PTR", "MX", "TXT"};
            return RR_TypeToStr[type];
        }

        friend std::ostream &operator<<(std::ostream &os, const ResourceRecord &r)
        {
            return os << "Domain: " << r.domain.c_str() << " | "
                      << "Type: " << r.typeToStr() << " | "
                      << "Ip: " << r.ip << " | ";
        }
    };

    // Defines the common association between URL and Resource Record
    using DomainRecordMap = std::map<opp_string, std::vector<ResourceRecord>>;

    /**
     * @brief Map that binds directly the domain to an Ip
     * @details Currently used with DnsServiceSimplified
     */
    using DirectDomainRecordMap = std::map<opp_string, ResourceRecord>;
}
#endif