package simcan2.Architecture.Nodes.Iot;

module LuminositySensor extends IotSensor {
	parameters:
	    @display("i=l-resistor64");
		model.tag = "luminosity";
		model.value = uniform(400,100000);
		model.unit = "lumen";
}	