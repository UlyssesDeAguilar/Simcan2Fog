package s2f.management.cloudprovider;

//
// Implementation of a system manager for cloud environments
//
simple CloudProviderService
{
    parameters:
        @class(CloudProvider);
        string dataManagerPath;
        string nodeDbPath;
        bool dispatchPriority = default(false); // Whether priority users skip the queue
    gates:
        input queueIn;
        output queueOut;
}