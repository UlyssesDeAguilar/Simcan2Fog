package s2f.apps.p2p.discovery.dns;
import s2f.apps.p2p.discovery.InnerThread;

simple DnsDiscoveryService extends InnerThread {
	parameters:
	  @class(DnsDiscoveryService);
		string dnsSeed @mutable;
		string dnsIp @mutable;
}
