package s2f.architecture.nodes.iot.controllers;

import inet.node.base.ApplicationLayerNodeBase;
import s2f.os.hardwaremanagers.EdgeHardwareManager;
import s2f.os.control.VmControlTable;
import s2f.architecture.cpu.CpuProcessor;
import s2f.os.hypervisors.EdgeHypervisor;
import s2f.os.schedulers.ICpuScheduler;
import s2f.os.schedulers.CpuSchedulerRR;
import s2f.architecture.disk.IDiskManager;
import s2f.architecture.disk.DiskManager;
import s2f.architecture.net.stack.NetworkStack;
import s2f.architecture.net.stack.resolver.DnsResolver;
import s2f.apps.AppsVector;

module EdgeNode extends ApplicationLayerNodeBase
{
    parameters:
        bool staticAppAssignment = default(true);            // It has to do with the dependency of the CheckingHubModule
        int maxApps = default(5);                           // Maximum app instances allowed

        // Hardware specs
        int numCpuCores = default(4);                        // The cores of the CPU equipped in this node
        int cpuSpeed = default(60000);						 // The speed (in MIPS) of each Core
        int cpuTick @unit(s) = default(60s);				 // The interval between CPU ticks (in seconds)
        double memorySize @unit(GB) = default(16GB);
        double diskSize @unit(GB) = default(500GB);
        double diskReadBandwidth @unit(Mbps) = default(800Mbps);
        double diskWriteBandwidth @unit(Mbps) = default(800Mbps);

        // Propagating values to submodules + configuration
        *.numCpuCores = this.numCpuCores;
        *.staticAppAssignment = this.staticAppAssignment;
        *.isVirtualHardware = false;

        // Setup of the cores
        cpu.cpuCoreType = "CpuCore";
        cpu.cpuCore[*].speed = this.cpuSpeed;
        cpu.cpuCore[*].tick = this.cpuTick;

		// App parameters
        apps.numApps = maxApps;
        apps.vmId = 0;
        apps.appModule[*].resolverPath = "^.^.^.resolver";

		// Pass settings down to the hardware manager
        hwm.maxVMs = 1;
        hwm.maxUsers = 1;
        hwm.memorySize = this.memorySize;
        hwm.diskSize = this.diskSize;

        // Hypervisor setup
        hy.active = true;
        hy.maxAppsPerVm = this.maxApps;
        hy.controlTablePath = "^.controlTable";
        hy.hardwareManagerPath = "^.hwm";
		controlTable.numVMs = 1;
		controlTable.numAppsPerVm = this.maxApps;
		
        // Disk setup
        disk.numVms = 1;
        disk.diskReadBandwidth = this.diskReadBandwidth;
        disk.diskWriteBandwidth = this.diskWriteBandwidth;

        @display("bgb=2153.4993,1269.8219");
    gates:
        input serialIn @loose;
        output serialOut @loose;

    submodules:
        //ns: NetworkStack {
            //@display("p=220,221");
            //numWlanInterfaces = default(1);
        	//energyStorage.typename = default("IdealEpEnergyStorage");
        	//wlan[0].typename = default("Ieee802154NarrowbandInterface");
        	//wlan[0].radio.energyConsumer.typename = default("SensorStateBasedEpEnergyConsumer"); //FIXME does this consumer exist???	
        //}

        hy: EdgeHypervisor {
            @display("p=1459.467,159.94159");
        }

        hwm: EdgeHardwareManager {
            @display("p=1916.443,399.85397;i=block/cogwheel");
        }

        cpu: CpuProcessor {
            @display("p=1461.0049,700.84296");
        }

        scheduler: <default("CpuSchedulerRR")> like ICpuScheduler {
            @display("p=1459.467,399.85397");
        }

        disk: <default("DiskManager")> like IDiskManager {
            @display("p=1727.9404,159.94159");
        }

        apps: AppsVector {
            @display("p=973.93005,59.978096");
        }

        controlTable: VmControlTable {
            @display("p=1702.2355,399.85397");
        }

        resolver: DnsResolver {
            @display("p=562.6517,59.978096");
        }
    connections:
        // Connect hypervisor to scheduler
        hy.toCpuScheduler++ --> scheduler.fromHypervisor;
        hy.fromCpuScheduler++ <-- scheduler.toHypervisor;

        // Connect CPU - Scheduler
        scheduler.toHub --> cpu.fromOs;
        scheduler.fromHub <-- cpu.toOs;

        // Connect apps to hypervisor
        hy.toApps++ --> apps.fromOS;
        hy.fromApps++ <-- apps.toOS;

        // Enable app connectivity to the socket interfaces
        for i=0..maxApps-1 {
            at.in++ <-- apps.socketOut++;
            at.out++ --> apps.socketIn++;
        }

        resolver.socketOut --> at.in++;
        resolver.socketIn <-- at.out++;

        hy.toDisk --> disk.diskIn;
        hy.fromDisk <-- disk.diskOut;

        hy.toNetwork --> at.in++;
        hy.fromNetwork <-- at.out++;

        hy.serialIn <-- serialIn;
        hy.serialOut --> serialOut;
}
