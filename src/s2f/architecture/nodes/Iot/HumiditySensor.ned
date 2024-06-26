package s2f.Architecture.Nodes.Iot;

module HumiditySensor extends IotSensor {
	parameters:
	    @display("i=water64");
		model.tag = "humidity";
		model.value = uniform(30,100);
		model.unit = "ratio";
}	