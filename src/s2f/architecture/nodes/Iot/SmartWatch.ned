package s2f.Architecture.Nodes.Iot;

module SmartWatch extends IotSensor {
	parameters:
	    @display("i=wearable64");
		model.tag = "heartbeat";
		model.value = uniform(50,100);
		model.unit = "bpm"; // Beats per minute
}	