package simcan2.Architecture.Nodes.Iot;
import simcan2.Architecture.Nodes.Iot.Internals.IotSensorModel;
import inet.node.inet.SensorNode;

module IotSensor extends SensorNode
{    
	submodules:
	    model: IotSensorModel;
	    
	connections:
	    at.in++ <-- model.socketOut;
	    at.out++ --> model.socketIn;
}