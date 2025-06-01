package s2f.architecture.dns.db;

//
// This is the implementation of a DNS database
// 
// This module can perform:
// - Insertion of DNS records
// - Deletion of DNS records
// - Retreival of DNS records
// 
//  This module can also preload DNS zones and records 
//  when passed into the zoneConfig parameter
//
//  Example config:
//  [
//      {
//          "zone": "fg-edge.com",
//          "records": [
//              {
//                  "domain": "*.fg-edge.com",
//                  "type": "A",
//                  "ip": "10.0.1.4"
//              }
//          ]
//      },
//      ...
//  ]
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
simple DnsDb
{
    parameters:
        object zoneConfig = default([]); // Zone configuration, expected in JSON like format
        @display("i=block/table,,0");
}
