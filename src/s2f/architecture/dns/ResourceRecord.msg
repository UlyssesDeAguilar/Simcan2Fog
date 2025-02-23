import inet.common.INETDefs;
import inet.networklayer.common.L3Address;

namespace dns;

enum RecordType
{
    A = 0;     //!< Address
    NS = 1;    //!< Name Server
    CNAME = 2; //!< Canonical Name
    SOA = 3;   //!< Start of Authority
    PTR = 4;   //!< Pointer
    MX = 5;    //!< eMail eXchange
    TXT = 6;   //!< Text string (arbitrary text)
    ANY = 7;   //!< All records
};

struct ResourceRecord
{
	string domain;
    string contents;
    inet::L3Address ip;
	RecordType type = A;
}