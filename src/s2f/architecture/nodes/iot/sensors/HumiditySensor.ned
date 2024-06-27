package s2f.architecture.nodes.iot.sensors;

module HumiditySensor extends IotSensor {
	parameters:
	    @display("i=water64");
		model.tag = "humidity";
		model.value = uniform(30,100);
		model.unit = "ratio";
}	