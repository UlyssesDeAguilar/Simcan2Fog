//package simcan2.tests.modules;

simple Endpoint
{
    parameters:
        @class(Endpoint);
        string nameToResolve;
    gates:
        input endpointIn;
        output endpointOut;
}