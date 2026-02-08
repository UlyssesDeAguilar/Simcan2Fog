#ifndef __S2F__DNS_RESOURCERECORD_H
#define __S2F__DNS_RESOURCERECORD_H

#include "s2f/architecture/dns/DnsDefinitions_m.h"
#include "s2f/architecture/dns/DnsName.h"
#include "s2f/architecture/dns/RecordData.h"

namespace s2f::dns
{
  class ResourceRecord final
  {
  protected:
    DnsName name{};
    RRType type = static_cast<s2f::dns::RRType>(-1);
    DnsClass dnsClass = INTERNET;
    uint32_t ttl = 0;
    RecordData data{};

  public:
    ResourceRecord() = default;

    /**
     * @brief Creates a new resource record
     * @details This constructor gives absolute control to the user, use with caution.
     * Also it's reccomended to use the other constructors if possible
     * @param domain Domain where it belongs
     * @param type Type of the record
     * @param dnsClass Class of the record
     * @param data Data of the record
     * @param ttl Time to live
     */
    ResourceRecord(const DnsName &domain, RRType type, DnsClass dnsClass, const RecordData &data, uint32_t ttl) : name(domain), type(type), dnsClass(dnsClass), ttl(ttl), data(data) {}

    /**
     * @brief Creates a new resource record for an A/AAAA record
     * @param domain Domain where it belongs
     * @param address IP address
     * @param ttl Time to live
     */
    ResourceRecord(const DnsName &domain, const inet::L3Address &address, uint32_t ttl);

    /**
     * @brief Creates a new resource record for a CNAME record
     * @param canonical_name Canonical name
     * @param domain Domain where it belongs
     * @param ttl Time to live
     */
    ResourceRecord(const DnsName &canonical_name, const DnsName &domain, uint32_t ttl);

    /**
     * @brief Creates a new resource record for a TXT record
     * @param domain Domain where it belongs
     * @param text Text
     * @param ttl Time to live
     */
    ResourceRecord(const DnsName &domain, const char *text, size_t text_length, uint32_t ttl);

    /**
     * @brief Creates a new resource record for a TXT record
     * @param domain Domain where it belongs
     * @param text Text
     * @param ttl Time to live
     */
    ResourceRecord(const DnsName &domain, const std::string &text, uint32_t ttl) : ResourceRecord(domain, text.c_str(), text.length(), ttl) {}

    /**
     * @brief Creates a new resource record for a TXT record
     * @param domain Domain where it belongs
     * @param text Text
     * @param ttl Time to live
     */
    ResourceRecord(const DnsName &domain, const omnetpp::opp_string &text, uint32_t ttl) : ResourceRecord(domain, text.c_str(), text.size(), ttl) {}

    const DnsName &getName() const { return name; }

    RRType getType() const { return type; }

    DnsClass getDnsClass() const { return dnsClass; }

    uint32_t getTtl() const { return ttl; }

    void setTtl(uint32_t ttl) { this->ttl = ttl; }

    const RecordData &getData() const { return data; }

    /**
     * @brief Returns the size of the resource record in bytes
     * @return The size of the resource record
     */
    size_t getSize() const;

    friend bool operator<(const ResourceRecord &a, const ResourceRecord &b) { return std::tie(a.dnsClass, a.type, a.name, a.data) < std::tie(b.dnsClass, b.type, b.name, b.data); }

    friend bool operator>(const ResourceRecord &a, const ResourceRecord &b) { return b < a; }

    friend bool operator<=(const ResourceRecord &a, const ResourceRecord &b) { return !(b < a); }

    friend bool operator>=(const ResourceRecord &a, const ResourceRecord &b) { return !(a < b); }

    friend bool operator==(const ResourceRecord &a, const ResourceRecord &b) { return std::tie(a.dnsClass, a.type, a.name, a.data) == std::tie(b.dnsClass, b.type, b.name, b.data); }

    friend bool operator!=(const ResourceRecord &a, const ResourceRecord &b) { return !(a == b); }

    friend std::ostream &operator<<(std::ostream &out, const ResourceRecord &record)
    {
      return out << "DNS Record: [ name: " << record.name
                 << ", type: " << record.type
                 << ", class: " << record.dnsClass
                 << ", ttl: " << record.ttl
                 << ", " << record.data << "]";
    }
  };

}

#endif // __S2F__DNS_RESOURCERECORD_H
