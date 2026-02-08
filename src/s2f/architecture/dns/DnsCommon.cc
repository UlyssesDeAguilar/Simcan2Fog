#include "s2f/architecture/dns/DnsCommon.h"

using namespace inet;

namespace s2f
{
    namespace dns
    {
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

            return opp_strcmp("FIMXE", tld) == 0;
        }

        bool matchWithWildcard(const ResourceRecord &record, const char *domain)
        {
            const char *r_domain = "FIXME";

            if (!domain)
                return false;

            if (r_domain[0] == '*' && r_domain[1] != '\0')
                return opp_stringendswith(domain, r_domain + 1);
            else
                return opp_strcmp(r_domain, domain) == 0;
        }
    }
}