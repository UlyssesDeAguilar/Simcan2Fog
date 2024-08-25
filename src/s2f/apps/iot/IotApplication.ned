package s2f.apps.iot;
import s2f.apps.AppBase;

// Application that is executed in a single computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple IotApplication extends AppBase{
    parameters:
        @class(IotApplication);
        int processingMIs = default(1);				         // Number of MIs to execute in each iteration
        int listeningPort = default(1000);					 // The port where it will listen (UDP)	
        int synthSize @unit(B) = default(40B);               // The payload that'll be sent

        bool serverClosesOnReturn = default(false);          // If the conexion should terminate once the response is back
        double replyDelay @unit(s) = default(0.04s);         // The time the server should take to process the request
        int replySize  @unit(B) = default(20B);              // The expected size of the reply
        
        string vmType = default("VM_small");
        int pingsThreshold = default(5);                     // The number of pings where it will start processing

        // These signals are only registering the output and input traffic from the endpoint
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @signal[roundTripTime](type=double);

        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
        @statistic[roundTripTime](title="round trip time"; unit=s; record=histogram,vector; interpolationmode=none);
}