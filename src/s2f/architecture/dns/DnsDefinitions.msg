cplusplus {{

constexpr size_t DNS_HEADER_SIZE = 12; // In bytes

}}


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