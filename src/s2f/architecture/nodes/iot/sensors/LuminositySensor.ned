package s2f.architecture.nodes.iot.sensors;

module LuminositySensor extends IotSensor {
	parameters:
	    @display("i=l-resistor64");
		model.tag = "luminosity";
		model.value = uniform(400,100000);
		model.unit = "lumen";
}	