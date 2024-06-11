package simcan2.Architecture.Nodes.Iot;
import inet.node.inet.WirelessHost;

module IotActuator extends WirelessHost{
    parameters:
        numApps = 1;
        app[0].typename = "UdpSink";
        app[0].localPort = 1000;
}