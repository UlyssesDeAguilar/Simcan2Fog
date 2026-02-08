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

#ifndef __S2F__DNS_RECORDDATA_H_
#define __S2F__DNS_RECORDDATA_H_

#include <cstdint>
#include <memory>
#include <omnetpp.h>

#include "inet/networklayer/common/L3Address.h"
#include "s2f/architecture/dns/DnsName.h"
#include "s2f/architecture/dns/DnsDefinitions_m.h"

namespace s2f::dns
{
    /**
     * @brief Represents the data field of a DNS resource record
     * @details It's designed to have the backing data inmutable, so it can be shared across multiple instances
     * with the added benefit of being able to interact almost hands free with the DnsName class.
     * @author Ulysses de Aguilar Gudmundsson
     * @version 1.0
     */
    class RecordData final
    {
    private:
        using DataField = std::shared_ptr<uint8_t[]>;
        uint16_t resourceDataLength{};    //!< Size of the RecordData field, in bytes
        DataField resourceData{};         //!< Data portion of the resource record
        DataMemoryType memoryType = NONE; //!< Data memory type, allows us to know how to interpret the data (Address, DNS formatted name or text)

    public:
        friend DnsName;

        /**
         * @brief Default constructor
         * @details Declared explicitly to default so that it can be instantiated on messages
         */
        RecordData() = default;

        /**
         * Constructor
         *
         * @param address L3Address to be initialized to
         */
        RecordData(const inet::L3Address &address);

        /**
         * Constructor
         *
         * @param name DnsName to be initialized to
         */
        RecordData(DnsName name);

        /**
         * Constructor
         *
         * @param text Text to be initialized to
         */
        RecordData(const char *text, size_t length);

        /**
         * Constructor
         *
         * @param text Text to be initialized to
         */
        RecordData(omnetpp::opp_string text) : RecordData(text.c_str(), text.size()) {}

        /**
         * Constructor
         *
         * @param text Text to be initialized to
         */
        RecordData(std::string text) : RecordData(text.c_str(), text.length()) {}

        /**
         * @brief Returns the length of the data field
         * @return The length of the data field
         */
        uint16_t getResourceDataLength() const { return resourceDataLength; }

        uint8_t getResourceDataByte(uint16_t index) const;

        inet::L3Address getDataAsAddress() const;

        DnsName getDataAsDnsName() const;

        omnetpp::opp_string getDataAsText() const;

        // Standard operator section

        RecordData &operator=(const DnsName &name);

        RecordData &operator=(const inet::L3Address &address);

        friend inline bool operator<(const RecordData &lhs, const RecordData &rhs)
        {
            int result = std::memcmp(lhs.resourceData.get(), rhs.resourceData.get(), std::min(lhs.resourceDataLength, rhs.resourceDataLength));
            return (result < 0) || (result == 0 && lhs.resourceDataLength < rhs.resourceDataLength);
        }

        friend inline bool operator>(const RecordData &lhs, const RecordData &rhs) { return rhs < lhs; }

        friend inline bool operator<=(const RecordData &lhs, const RecordData &rhs) { return !(lhs > rhs); }

        friend inline bool operator>=(const RecordData &lhs, const RecordData &rhs) { return !(lhs < rhs); }

        friend inline bool operator==(const RecordData &lhs, const RecordData &rhs)
        {
            // If the internal memory is the same, then it's equal
            return lhs.resourceData.get() == rhs.resourceData.get() ||
                   (lhs.resourceDataLength == rhs.resourceDataLength &&
                    // Since we made sure the length is the same we can use lhs or rhs length safely
                    std::memcmp(lhs.resourceData.get(),
                                rhs.resourceData.get(),
                                lhs.resourceDataLength) == 0);
        }

        friend inline bool operator!=(const RecordData &lhs, const RecordData &rhs) { return !(lhs == rhs); }

        friend std::ostream &operator<<(std::ostream &os, const RecordData &record)
        {
            return os << "RData : [ length: "
                      << record.resourceDataLength
                      << " data: ";

            switch (record.memoryType)
            {
            case DNS_NAME:
                return os << record.getDataAsDnsName();
            case ADDR:
                return os << record.getDataAsAddress();
            case TEXT:
                return os << record.getDataAsText();
            default:
                return os << "NO DATA";
            }

            return os << " ]";
        }
    };
}

#endif /* __S2F__DNS_RECORDDATA_H_ */