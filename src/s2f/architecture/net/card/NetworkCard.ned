package s2f.architecture.net.card;

simple NetworkCard
{
    parameters:
        int L2Address @mutable = default(-1);
        bool gratuitousBroadcastOnStart = default(true);
        @display("i=device/card,,0");
    gates:
        inout comm;
        input cardIn;
        output cardOut;
}
