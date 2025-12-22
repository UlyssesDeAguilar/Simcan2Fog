
package s2f.apps.p2p.discovery.hardcoded;
import s2f.apps.p2p.discovery.InnerThread;

simple HardcodedDiscoveryService extends InnerThread {
	parameters:
	  @class(HardcodedDiscoveryService);
    object addressList = default(["10.0.0.1", "10.0.0.2"]); // List of strings with IP Addresses.
}
