package s2f.apps.p2p.discovery.dns;
import s2f.apps.p2p.discovery.DiscoveryService;

simple DnsDiscoveryService extends DiscoveryService {
	parameters:
	  @class(DnsDiscoveryService);
		string dnsSeed @mutable;
		string dnsIp @mutable;
}
