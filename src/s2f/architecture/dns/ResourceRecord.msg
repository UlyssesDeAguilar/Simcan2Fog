import inet.common.INETDefs;
import inet.networklayer.common.L3Address;

namespace s2f::dns;

//
// Resource record/question types
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
enum RecordType
{
    A = 0;     // Address
    NS = 1;    // Name Server
    CNAME = 2; // Canonical Name
    SOA = 3;   // Start of Authority
    PTR = 4;   // Pointer
    MX = 5;    // eMail eXchange
    TXT = 6;   // Text string (arbitrary text)
    ANY = 7;   // All records (should only be used on queries)
};

//
// DNS resource record
//
// @author Ulysses de Aguilar Gudmundsson
// @version 2.0
struct ResourceRecord
{
	string domain; // Domain where it belongs
    string contents; // Text contents, only makes sense with PTR/CNAME records
    inet::L3Address ip; // IP address contained (unless it's a PTR/CNAME record)
	RecordType type = A; // Record type
}