package simcan2.Applications.Services;
import inet.applications.contract.IApp;

simple ProcessingService like IApp
{
    parameters:
        string localAddress = default(""); // local address; may be left empty ("")
        int localPort = default(1000);     // localPort number to listen on
        double replyDelay @unit(s) = default(0s);
        
        string resourceManagerPath = default("^.^.resourceManager");
        string dcManagerPath = default("^.^.dcManager");
        
        int scalingThreshold = default(1000);
        string vmType = default("vmSmall");
        int minimumVms = default(4);
        string selectionStrategy = default("FirstFit");

        @display("i=block/app");
        @lifecycleSupport;
        double stopOperationExtraTime @unit(s) = default(-1s);    // extra time after lifecycle stop operation finished
        double stopOperationTimeout @unit(s) = default(2s);    // timeout value for lifecycle stop operation

        @signal[connections](type=long);
        @signal[capacity](type=double);
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        
        @statistic[concurrentConnections](title="concurrent connections"; source=connections; record=vector; interpolationmode=none);
        @statistic[capacity](title="capacity percentile"; source=capacity; record=vector; interpolationmode=none);

        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
    gates:
        input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}