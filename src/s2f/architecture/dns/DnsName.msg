cplusplus{{
#include "s2f/architecture/dns/DnsName.h"
}}

namespace s2f::dns;


//
// DNS domain name
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
class DnsName {
    @existingClass;
    @descriptor(readonly);
    uint8_t length;
    uint8_t numTags;
    bool dotFormat @getter("isDotFormat");
    uint8_t data[] @sizeGetter("getLength");
}