/**
 * @file common.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines and concentrates common concepts of the DNS protocol
 * @version 1.0
 * @date 2025-02-02
 */

#ifndef SIMCAN_EX_DNS_COMMON
#define SIMCAN_EX_DNS_COMMON

#include <memory>
#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"
#include "s2f/architecture/dns/ResourceRecord_m.h"

namespace dns
{
    /*
        Based on the RFC 1034/1035 and the explanation found in:
        http://www.tcpipguide.com/free/t_DNSNameServerDataStorageResourceRecordsandClasses-3.htm#Table_166
    */

    constexpr uint32_t DNS_PORT = 53;                          //!< The standard DNS port
    static const inet::Ipv4Address ROOT_DNS_IP("199.7.83.42"); //!< Root DNS server IP (in reality is the I root server - ICANN)
}

#endif