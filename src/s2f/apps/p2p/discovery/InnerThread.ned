package s2f.apps.p2p.discovery;
import s2f.apps.AppBase;

//
// Internal thread ran by an application.
// Communicates by receiving InnerRequest messages through sendDirect
//
simple InnerThread extends AppBase {
    gates:
        input internal @directIn;
}
