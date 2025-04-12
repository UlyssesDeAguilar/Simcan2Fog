package s2f.management.managers;

// This module is the "resource control table" of an Dc or Fog node
// Remember to include it at the root level of the Datacentre
// 
simple ResourceManager{
    parameters:
        @display("i=block/cogwheel");
        @class(DcResourceManager);
        string nodePoolPath;
        string dataManagerPath;
        string selectionStrategy;
        
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
        @statistic[allocatedRam](source=allocatedRam;title="Memoria RAM reservada"; unit=GB; record=stats,vector);
        @statistic[allocatedDisk](source=allocatedDisk;title="Almacenamiento reservado"; unit=GB; record=stats,vector);
        @statistic[allocatedCores](source=allocatedCores;title="CPUs reservadas"; record=stats,vector);
        @statistic[allocatedVms](source=allocatedVms;title="VMs reservadas"; record=stats,vector);
}
