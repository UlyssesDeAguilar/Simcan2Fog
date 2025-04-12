package s2f.architecture.nodes.iot.sensors;
import inet.node.inet.WirelessHost;

module IotSensor extends WirelessHost
{
    parameters:
        volatile bool powerOn @mutable = default(false);
        model.powerOn = this.powerOn;
    submodules:
        model: IotSensorModel;
    connections:
        at.in++ <-- model.socketOut;
        at.out++ --> model.socketIn;
}
