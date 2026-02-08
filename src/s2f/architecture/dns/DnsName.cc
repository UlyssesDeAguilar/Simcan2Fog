#include "s2f/architecture/dns/DnsName.h"
#include "s2f/architecture/dns/DnsDefinitions_m.h"
#include "s2f/architecture/dns/RecordData.h"
#include <cctype>

using namespace omnetpp;
using namespace s2f::dns;

bool DnsName::validTagCharacter(const char c)
{
    return std::isalnum(c) || c == '-' || c == '.' || c == '*';
}

DnsName::DnsName(const char *name, size_t str_length)
{
    constexpr size_t NAME_MAX_LENGTH = DNS_NAME_MAX_LENGTH - 2;
    uint8_t buffer[DNS_NAME_MAX_LENGTH];

    if (name == nullptr)
        throw cRuntimeError("Name cannot be nullptr");

    // Max length is 255 bytes, we take out 1 for null terminator and 1 for the first tag
    if (str_length == 0 || str_length > NAME_MAX_LENGTH)
        throw cRuntimeError("Invalid DNS name length, cannot be zero or greater than %zu", NAME_MAX_LENGTH);

    // Avoid null tags
    if (name[0] == '.')
        throw cRuntimeError("Invalid DNS name, first tag may not be nill");

    // Adjustment for edge case of ending with a dot
    if (name[str_length - 1] == '.')
        str_length--;

    // Test for edge case google..
    if (name[str_length - 1] == '.')
        throw cRuntimeError("Invalid DNS name, last tag may not be nill");

    // Validate the name meanwhile we get the delimeters
    uint8_t j = 1, tagLength = 0, tagPosition = 0;

    // str_length <= 253 -> max(j) = max(i) + 1 = 253 -> no buffer overflow (we have space for nil + first tag)
    for (uint8_t i = 0; i < str_length; i++, j++)
    {
        if (!validTagCharacter(name[i]))
            throw cRuntimeError("Invalid DNS tag character: %c", name[i]);

        if (name[i] == '.')
        {
            // This avoids the intermediate situation of google..com, which is not valid
            if (tagLength == 0)
                throw cRuntimeError("Invalid DNS name, intermediate null tags are not allowed");

            buffer[tagPosition] = tagLength;
            tagPosition = j;
            tagLength = 0;
            numTags++;
        }
        else
        {
            // This canonicalizes the names as the DNS names are supposed to be case insensitive
            buffer[j] = std::isalpha(name[i]) ? std::tolower(name[i]) : name[i];
            tagLength++;
        }

        if (tagLength > DNS_TAG_MAX_LENGTH)
            throw cRuntimeError("Invalid DNS tag length, cannot be greater than %zu", DNS_TAG_MAX_LENGTH);
    }

    // Add the last tag
    numTags++;

    // Last tag must be filled in
    buffer[tagPosition] = tagLength;

    // Length is the string length + 2
    length = str_length + 2;

    // Mark with null terminator
    buffer[length - 1] = 0;

    // Copy the buffer to the data
    data = Data(new uint8_t[length], std::default_delete<uint8_t[]>());
    std::memcpy(data.get(), buffer, length);
}

DnsName::DnsName(RecordData recordData) : DnsName()
{
    data = recordData.resourceData;
    length = recordData.resourceDataLength;

    // Go over the fields and count the number of tags
    uint8_t tagPosition = 0;
    while (tagPosition < length)
    {
        tagPosition += data[tagPosition] + 1;
        numTags++;
    }
}

opp_string DnsName::getTag(uint8_t index) const
{
    char buffer[DNS_TAG_MAX_LENGTH];

    if (data == nullptr || index >= numTags)
        throw cRuntimeError("Invalid DNS name index: %d", index);

    uint8_t tagPosition = 0, i = 0;
    while (i++ < index)
        tagPosition += data[tagPosition] + 1;

    size_t length = data[tagPosition];
    std::memcpy(buffer, data.get() + tagPosition + 1, length);
    buffer[length] = '\0';

    return opp_string(buffer);
}

bool DnsName::isWildcard() const
{
    if (data == nullptr || numTags < 2)
        return false;

    uint16_t bytes = data[0] << 8 | data[1];
    return (bytes & (~0x012A)) == 0;
}

std::ostream &DnsName::print(std::ostream &os) const
{
    char buffer[8];

    for (uint8_t i = 0, tagPosition = 0; i < length; i++)
    {
        if (i == tagPosition)
        {
            if (dotFormat)
                os << ".";
            else
            {
                snprintf(buffer, sizeof(buffer) * sizeof(buffer[0]), "[%d]", data[i]);
                os << buffer;
            }
            
            // Current position + length + 1 -> next tag!
            tagPosition = i + data[tagPosition] + 1;
        }
        else
            os << data[i];
    }

    return os;
}
