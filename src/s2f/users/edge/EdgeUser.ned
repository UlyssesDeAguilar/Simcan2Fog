package s2f.users.edge;

simple EdgeUser
{
    parameters:
        @class(users::EdgeUser);
		string deploymentZone;
		object vms;
		object remoteApps;
		object localApps;
		object sensors;
		double startupTime @unit(seconds) = default(1s);
		string controllerHostName = default("localhost");
        bool makeServiceNameUnique = default(true);
        @display("i=user,,0");
    gates:
        input queueIn @loose;
        input serialIn;

        output queueOut @loose;
        output serialOut;
}
