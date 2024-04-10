package simcan2.Management.DataCentreManagers.ResourceManager;

// This module is the "resource control table" of an Dc or Fog node
// Remember to include it at the root level of the Datacentre
// 
simple DcResourceManager{
    parameters:
        @class(DcResourceManager);
        
        // Signals emmited from the model
        
        // For initialization purposes
        @signal[maxRam](type=double);
        @signal[maxDisk](type=double);
        @signal[maxCores](type=unsigned long);
        @signal[maxVms](type=unsigned long);

        // Actual live data
        @signal[allocatedRam](type=double);
        @signal[allocatedDisk](type=double);
        @signal[allocatedCores](type=long);
        @signal[allocatedVms](type=long);
        

        // Characteristical statistics
        @statistic[maxRam](unit=GB; record=last);
        @statistic[maxDisk](unit=GB; record=last);
        @statistic[maxCores](record=last);
        @statistic[maxVms](record=last);

        // Live statistics
        @statistic[allocatedRam](title="Memoria RAM reservada"; unit=GB; record=min,max,vector);
        @statistic[allocatedDisk](title="Almacenamiento reservado"; unit=GB; record=min,max,vector);
        @statistic[allocatedCores](title="CPUs reservadas"; record=min,max,vector);
        @statistic[allocatedVms](title="VMs reservadas"; record=min,max,vector);

}
