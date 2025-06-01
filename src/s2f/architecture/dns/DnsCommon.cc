#include "s2f/architecture/dns/DnsCommon.h"

using namespace inet;

namespace s2f
{
    namespace dns
    {
        std::ostream &operator<<(std::ostream &os, const ResourceRecord &obj)
        {
            return os << "Record :  [ domain: " << obj.domain
                      << " | type: " << obj.type
                      << " | ip: " << obj.ip
                      << " | text: " << obj.contents
                      << " ]";
        }

        std::ostream &operator<<(std::ostream &os, const RecordType type)
        {
            switch (type)
            {
            case RecordType::A:
                return os << "A";
            case RecordType::NS:
                return os << "NS";
            case RecordType::CNAME:
                return os << "CNAME";
            case RecordType::SOA:
                return os << "SOA";
            case RecordType::PTR:
                return os << "PTR";
            case RecordType::MX:
                return os << "MX";
            case RecordType::TXT:
                return os << "TXT";
            case RecordType::ANY:
                return os << "ANY";
            }
            return os << "UNKNOWN";
        }

        RecordType getRecordType(const char *type)
        {
            if (strncmp(type, "A", 1) == 0)
                return RecordType::A;
            if (strncmp(type, "NS", 2) == 0)
                return RecordType::NS;
            if (strncmp(type, "CNAME", 5) == 0)
                return RecordType::CNAME;
            if (strncmp(type, "SOA", 3) == 0)
                return RecordType::SOA;
            if (strncmp(type, "PTR", 3) == 0)
                return RecordType::PTR;
            if (strncmp(type, "MX", 2) == 0)
                return RecordType::MX;
            if (strncmp(type, "TXT", 3) == 0)
                return RecordType::TXT;
            if (strncmp(type, "ANY", 3) == 0)
                return RecordType::ANY;

            throw cRuntimeError("Unknown record type: %s", type);
            // This is to silence compiler warnings
            return RecordType::ANY;
        }

        opp_string getQuestionForLevel(const char *domain, DnsLevel level)
        {
            const char *tld = domain, *root = nullptr;

            for (const char *p = domain; p != nullptr && *p != '\0'; p++)
            {
                if (*p == '.' && *p != '\0')
                {
                    tld = root;
                    root = p;
                }
            }

            if (root == nullptr)
                throw cRuntimeError("Invalid FQDN: %s", domain);

            if (level == TLD)
                return tld + 1;
            else if (level == ROOT)
                return root + 1;
            else
                return domain;
        }

        bool matchesAuthorityDomain(const ResourceRecord &record, const char *domain)
        {
            const char *tld = domain, *root = nullptr;

            for (const char *p = domain; p != nullptr && *p != '\0'; p++)
            {
                if (*p == '.' && *p != '\0')
                {
                    tld = root;
                    root = p;
                }
            }

            if (root == nullptr)
                throw cRuntimeError("Invalid FQDN: %s", domain);

            return opp_strcmp(record.domain.c_str(), tld) == 0;
        }

        bool matchWithWildcard(const ResourceRecord &record, const char *domain)
        {
            const char *r_domain = record.domain.c_str();

            if (!domain)
                return false;

            if (r_domain[0] == '*' && r_domain[1] != '\0')
                return opp_stringendswith(domain, r_domain + 1);
            else
                return opp_strcmp(r_domain, domain) == 0;
        }
    }
}