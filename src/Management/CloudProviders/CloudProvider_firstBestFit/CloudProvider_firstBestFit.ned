package simcan2.Management.CloudProviders.CloudProvider_firstBestFit;
import simcan2.Management.CloudProviders.CloudProviderBase_firstBestFit.CloudProviderBase_firstBestFit;

//
// Implementation of a system manager for cloud environments
//
simple CloudProvider_firstBestFit extends CloudProviderBase_firstBestFit{
    parameters:    
        bool checkReservedFirst = default (false);
    @class(CloudProvider_firstBestFit);	
}
