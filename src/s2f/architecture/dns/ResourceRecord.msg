cplusplus {{
#include "s2f/architecture/dns/ResourceRecord.h"
}}

import s2f.architecture.dns.DnsDefinitions;
import s2f.architecture.dns.DnsName;
import s2f.architecture.dns.RecordData;

namespace s2f::dns;

//
// DNS resource record
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
class ResourceRecord
{
    @existingClass;
    @descriptor(readonly);
	DnsName name; // Domain where it belongs (variable size)
    RRType type; // Type of the record, as described in RecordType (2 Bytes)
    DnsClass dnsClass = INTERNET; // Type of class, as described in RecordClass (usually 1 for IN == Internet) (2 Bytes)
	uint32_t ttl; // Time to live of the record, in seconds (4 Bytes)
    RecordData data; // Combines the fields resourceDataLength and the data itself (2 Bytes + variable)
}