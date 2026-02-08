//
// All of the values included in this definition file can be
// obtanied and/or updated from: https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0

namespace s2f::dns;

cplusplus {{
constexpr size_t DNS_HEADER_SIZE = 12; // In bytes
constexpr uint16_t DNS_PORT = 53; // Port of the DNS protocol, available on both UDP/TCP
constexpr size_t DNS_NAME_MAX_LENGTH = 255; // Maximum octets for dns names as described in RFC 1035 section 2.3.4
constexpr size_t DNS_TAG_MAX_LENGTH = 63; // Maximum octets for dns tags as described in RFC 1035 section 2.3.4

// Constants for Resource Record field sizes
constexpr size_t RR_TYPE_SIZE = 2;
constexpr size_t RR_CLASS_SIZE = 2;
constexpr size_t RR_TTL_SIZE = 4;
constexpr size_t RR_DATA_LEN_SIZE = 2;

template <typename T>
typename std::enable_if<std::is_enum<T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& e) {
    return os << static_cast<typename std::underlying_type<T>::type>(e);
}

}}

//
// DNS general administrative domains
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum DnsLevel
{
    ROOT = 0; // Root level, usually managed by ICANN
    TLD = 1; // Top Level Domain level, managed either by ICANN or Countries Goverments
    AUTHORITATIVE = 2; // Authoritative level, managed by the entity (university, goverment, enterprises, ...)
};

//
// DNS classes
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum DnsClass
{
    RESERVED = 0; // Reserved as in RFC 6895
    INTERNET = 1; // Internet as in RFC 1035
    UNASSIGNED = 2; // Unassinged by IETF
    CHAOS = 3; // CH class, referenced from RFC 6895
    HESIOD = 4; // HS class, referenced from RFC 6895
};

//
// DNS header field for operation codes
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum OperationCode
{
    QUERY  = 0;  // Standard query
    IQUERY = 1;  // Used for inverse querys 				(obsolete)
    STATUS = 2;  // Request for status					(currently not used)
    NOTIFY = 3;  // Notify for Zone Transfer				(currently not used)
};

//
// DNS header field for return codes
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
enum ReturnCode
{
    NOERROR  = 0;  // DNS Query completed successfully
    FORMERR  = 1;  // DNS Query Format Error
    SERVFAIL = 2;  // Server failed to complete the DNS request
    NXDOMAIN = 3;  // Domain name does not exist
    NOTIMP	 = 4;  // Function not implemented
    REFUSED  = 5;  // The server refused to answer for the query (currently not used)
    YXDOMAIN = 6;  // Name that should not exist, does exist	  (currently not used)
    XRRSET   = 7;  // RRset that should not exist, does exist	  (currently not used)
    NOTAUTH  = 8;  // Server not authoritative for the zone	  (currently not used)
    NOTZONE  = 9;  // Name not in zone							  (currently not used)
};

//
// Resource record types
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
enum RRType
{
    @class; // Ensures this is an enum class (avoids name colision with QType)
    A = 0; // IPv4 Address
    NS = 1; // Name Server
    CNAME = 5; // Canonical Name
    SOA = 6; // Start of Authority
    MB = 7; // Mail box domain name (EXPERIMENTAL)
    MG = 8; // Mail group member (EXPERIMENTAL)
    MR = 9; // Mail rename domain name (EXPERIMENTAL)
    NULL_RR = 10; // Null RR (EXPERIMENTAL)
    WKS = 11; // Well known service
    PTR = 12; // Pointer
    HINFO = 13; // Host information
    MINFO = 14; // Mailbox or mail list information
    MX = 15; // eMail eXchange
    AAAA = 28; // IPv6 Address
    TXT = 16; // Text string (arbitrary text)
};

//
// Question types
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
enum QType
{
    @class; // Ensures this is an enum class (avoids name colision with RRType)
    A = 0; // IPv4 Address
    NS = 1; // Name Server
    CNAME = 5; // Canonical Name
    SOA = 6; // Start of Authority
    MB = 7; // Mail box domain name (EXPERIMENTAL)
    MG = 8; // Mail group member (EXPERIMENTAL)
    MR = 9; // Mail rename domain name (EXPERIMENTAL)
    NULL_RR = 10; // Null RR (EXPERIMENTAL)
    WKS = 11; // Well known service
    PTR = 12; // Pointer
    HINFO = 13; // Host information
    MINFO = 14; // Mailbox or mail list information
    MX = 15; // eMail eXchange
    TXT = 16; // Text string (arbitrary text)
    AAAA = 28; // IPv6 Address
    WILDCARD = 255; // Request for some/all available records
};

//
// Internal data memory types for the RecordData class
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
enum DataMemoryType
{
    NONE = 0; // If there's no data yet
    ADDR = 1; // Address (IPv4 or IPv6)
    DNS_NAME = 2; // DNS formatted name
    TEXT = 3; // Arbitrary text
}
