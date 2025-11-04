import inet.common.INETDefs;
import inet.networklayer.common.L3Address;

message StubDnsRequest
{
    int moduleId;
    string domain;
}

message StubDnsResponse
{
    int result;
    inet::L3Address address[];
    string domain;
}
