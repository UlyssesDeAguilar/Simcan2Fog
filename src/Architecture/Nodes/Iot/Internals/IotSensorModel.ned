package simcan2.Architecture.Nodes.Iot.Internals;
import inet.applications.contract.IApp;

simple IotSensorModel like IApp
{
    parameters:
        string tag;
        double value;
        string unit;
        int updatePeriod @unit(s);
        string controllerPath = default("^.^.controller");
        int listeningPort = default(-1);
        int destinationPort = default(1000);
        double startTime @unit(s) = uniform(0.5, 5);       // application start time (start of the first packet)
        double stopTime @unit(s) = default(-1s);  	       // time of finishing sending, -1s means forever
        
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[throughput](title="throughput"; unit=bps; source="throughput(packetReceived)"; record=vector);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[rcvdPkLifetime](title="received packet lifetime"; source="dataAge(packetReceived)"; unit=s; record=stats,vector; interpolationmode=none);
        //@statistic[rcvdPkSeqNo](title="received packet sequence number"; source="appPkSeqNo(packetReceived)"; record=vector; interpolationmode=none);
        
    gates:
        input socketIn;
        output socketOut;
}
