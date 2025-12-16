package s2f.apps.p2p.discovery.dns;
import s2f.apps.AppBase;

simple DnsDiscoveryService extends AppBase {
	parameters:
	  @class(DnsDiscoveryService);
		int listeningPort;
		string dnsSeed @mutable;
		string dnsIp @mutable;
}
