package simcan2.Architecture.Nodes.Iot;

module IpCamera extends IotSensor {
	parameters:
	    //@display("i=wearable64");
		model.tag = "video";
		model.value = uniform(2,3);
		model.unit = "MBit";
		model.updatePeriod = 1s;
}	