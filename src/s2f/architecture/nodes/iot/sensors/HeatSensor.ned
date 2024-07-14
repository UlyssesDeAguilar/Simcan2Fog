package s2f.architecture.nodes.iot.sensors;

module HeatSensor extends IotSensor {
	parameters:
	    @display("i=temperature64");
		model.tag = "temperature";
		model.value = uniform(20,25);
		model.unit = "ºC";
}