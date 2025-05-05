package s2f.architecture.nodes.iot.sensors;

module IpCamera extends IotSensor
{
    parameters:
        model.tag = "video";
        model.value = uniform(2,3);
        model.unit = "MBit";
        model.updatePeriod = 1s;
        @display("i=misc/securitycamera,,0;bgb=,,,,0");
}
