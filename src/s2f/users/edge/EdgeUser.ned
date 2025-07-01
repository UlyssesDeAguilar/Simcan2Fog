package s2f.users.edge;

//
// This module captures the behaviour of an edge user.
//
// @author Ulysses de Aguilar Gudmundsson
// @date 2025-05-03
// @version 1.0
simple EdgeUser
{
    parameters:
		string deploymentZone; // Preferred deployment zone
		object vms; // JSON like array with the VM definitions
		object remoteApps; // JSON like array with the app definitions that will be deployed remotely
		object localApps; // JSON like array with the app definitions that will be deployed locally
		object sensors; // JSON like array of expressions that identify the managed sensors
		double startupTime @unit(seconds) = default(1s); // Time to startup the user
		string controllerHostName = default("localhost"); // Local PC/controller hostname
        bool makeServiceNameUnique = default(true); // Whether to mix in the vmIds and appIds the users moduleId
        @display("i=user,,0");
    gates:
        input queueIn @loose; // Input from the message queue
        input serialIn; // Input from the local PC/controller

        output queueOut @loose; // Output to the message queue
        output serialOut; // Output to the local PC/controller
}
