
package s2f.apps.p2p;
import s2f.apps.UserAppModule;

module P2Protocol extends UserAppModule {
  gates:
    // Derived functionality
    input internalIn[];
    output internalOut[];

    // Discovery services
    input discoveryIn[];
    output discoveryOut[];

}
