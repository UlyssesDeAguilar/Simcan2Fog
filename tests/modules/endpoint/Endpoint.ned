package endpoint;

simple Endpoint
{
    parameters:
        @class(Endpoint);
        string nameToResolve;
    gates:
        input endpointIn;
        output endpointOut;
}