package s2f.architecture.dns.db;

simple DnsDb
{
    parameters:
        @class(dns::DnsDb);
        object zoneConfig = default([]);
        @display("i=block/table,,0");
}
