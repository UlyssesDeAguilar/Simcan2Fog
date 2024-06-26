package s2f.Architecture.Nodes.Iot;
import s2f.Architecture.Nodes.Iot.Internals.IotSensorModel;
import inet.node.inet.WirelessHost;

module IotSensor extends WirelessHost
{
    submodules:
        model: IotSensorModel;
    connections:
        at.in++ <-- model.socketOut;
        at.out++ --> model.socketIn;
}
