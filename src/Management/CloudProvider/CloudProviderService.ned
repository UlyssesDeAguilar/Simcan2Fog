package simcan2.Management.CloudProvider;

//
// Implementation of a system manager for cloud environments
//
simple CloudProviderService
{
    parameters:
        @class(CloudProvider);
        bool dispatchPriority = default(false); // Whether priority users skip the queue
    gates:
        input queueIn;
        output queueOut;
        input listener;
}