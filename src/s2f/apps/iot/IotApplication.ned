package s2f.apps.iot;
import s2f.apps.AppBase;

// Application model for IoT controller logic
//
// @author Ulysses de Aguilar
// @date 2025-05-01
// @version 2.0
simple IotApplication extends AppBase{
    parameters:
        @class(IotApplication);
        string parentPath = default("^.^.^.^.");
        int processingMIs = default(1);				         // Number of MIs to execute in each iteration
        int listeningPort = default(1000);					 // The port where it will listen (UDP)	
        int synthSize @unit(B) = default(40B);               // The payload that'll be sent
        int pingsThreshold = default(5);                     // The number of pings where it will start processing
        string localInterface = default("");
        string externalInterface = default("");
        string serviceName @mutable;
        double pollingInterval @unit(s) = default(5s);

        // These signals are only registering the output and input traffic from the endpoint
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @signal[roundTripTime](type=double);

        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        @statistic[roundTripTime](title="round trip time"; unit=s; record=histogram,vector; interpolationmode=none);
}