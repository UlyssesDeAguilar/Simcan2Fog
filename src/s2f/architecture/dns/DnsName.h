//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __S2F__DNS_DNSNAME_H_
#define __S2F__DNS_DNSNAME_H_

#include <omnetpp.h>
#include <cstdint>
#include <memory>

namespace s2f::dns
{
    class RecordData; // Forward declaration

    class DnsName final
    {
        friend class DnsNameDescriptor;
        friend class RecordData;

    protected:
        using Data = std::shared_ptr<uint8_t[]>;
        Data data{};         // DNS name notation in binary format
        uint8_t length = 0;  // Length of the binary data
        uint8_t numTags = 0; // Number of tags present on the DNS name
        bool dotFormat = false;

        /**
         * Constructor from a binary DNS name
         * @details The dns_name field is expected to have been parsed by the DnsName default constructor.
         * That's why this method is protected and used only by friend classes that are expected to respect the format.
         *
         * @param dns_name Binary DNS name
         * @param length Length of the binary DNS name
         */
        DnsName(RecordData recordData);

        uint8_t getData(size_t index) const { return data[index]; }

    public:
        /**
         * Default constructor
         */
        DnsName() = default;

        /**
         * Constructor from a c string
         *
         * @param name DNS name to be parsed
         * @param str_length Length of the DNS name
         * @throws cRuntimeError If the DNS name is invalid
         */
        DnsName(const char *name, size_t str_length);

        /**
         * Constructor from an omnetpp::opp_string
         *
         * @param name DNS name to be parsed
         */
        DnsName(const omnetpp::opp_string &name) : DnsName(name.c_str(), name.size()) {}

        /**
         * Constructor from a std::string
         *
         * @param name DNS name to be parsed
         */
        DnsName(const std::string &name) : DnsName(name.c_str(), name.size()) {}

        /**
         * Get the length of the DNS name in bytes
         *
         * @return Length of the DNS name (including the null terminator)
         */
        uint8_t getLength() const { return length; }

        /**
         * Get the number of tags in the DNS name
         *
         * @return Number of tags (maximum 127 tags)
         */
        uint8_t getNumTags() const { return numTags; }

        /**
         * Builds a representation of a DNS tag into a opp_string
         *
         * @param index Index of the tag
         * @return The opp_string representation of the tag
         */
        omnetpp::opp_string getTag(uint8_t index) const;

        /**
         * Check if the DNS name contains a "wilcard domain name"
         * as the criteria established in RFC 4592
         *
         * @return True if it is a wildcard domain name
         */
        bool isWildcard() const;

        // void addTag(const char *tag);

        /**
         * Check if the character is a valid DNS tag character
         *
         * @return True if the character is a valid DNS tag character
         */
        static inline bool validTagCharacter(const char c);

        /**
         * Set if the print method should output the DNS name in dot format
         *
         * @param dotFormat True if the DNS name should be printed in dot format
         */
        void setDotFormat(bool dotFormat) { this->dotFormat = dotFormat; }

        /**
         * Get if the print method should output the DNS name in dot format
         *
         * @return True if the DNS name should be printed in dot format
         */
        bool isDotFormat() const { return dotFormat; }

        /**
         * Print the DNS name in a printable representation
         *
         * @param out Output stream
         * @return The output stream
         */
        std::ostream &print(std::ostream &out) const;

        friend inline bool operator<(const DnsName &lhs, const DnsName &rhs)
        {
            int result = std::memcmp(lhs.data.get(), rhs.data.get(), std::min(lhs.length, rhs.length));
            return (result < 0) || (result == 0 && lhs.length < rhs.length);
        }

        friend inline bool operator>(const DnsName &lhs, const DnsName &rhs) { return rhs < lhs; }

        friend inline bool operator<=(const DnsName &lhs, const DnsName &rhs) { return !(lhs > rhs); }

        friend inline bool operator>=(const DnsName &lhs, const DnsName &rhs) { return !(lhs < rhs); }

        friend inline bool operator==(const DnsName &lhs, const DnsName &rhs)
        {
            return lhs.length == rhs.length &&
                   std::memcmp(lhs.data.get(),
                               rhs.data.get(),
                               std::min(lhs.length, rhs.length)) == 0;
        }

        friend inline bool operator!=(const DnsName &lhs, const DnsName &rhs) { return !(lhs == rhs); }
    };

    inline std::ostream &operator<<(std::ostream &out, const DnsName &name) { return name.print(out); }
}
#endif /* __S2F__DNS_DNSNAME_H_ */