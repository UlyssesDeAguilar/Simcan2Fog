package s2f.architecture.nodes.iot.sensors;
import inet.node.inet.WirelessHost;

module IotSensor extends WirelessHost
{
    submodules:
        model: IotSensorModel;
    connections:
        at.in++ <-- model.socketOut;
        at.out++ --> model.socketIn;
}
