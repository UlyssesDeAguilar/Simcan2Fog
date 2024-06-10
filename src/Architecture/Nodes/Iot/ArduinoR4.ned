package simcan2.Architecture.Nodes.Iot;

import simcan2.Architecture.Nodes.HardwareManagers.HardwareManager.HardwareManager;
import simcan2.Architecture.Cpu.CpuProcessor.CpuProcessor;
import simcan2.OperatingSystem.Hypervisors.EdgeHypervisor.EdgeHypervisor;
import simcan2.OperatingSystem.CpuSchedulers.ICpuScheduler;
import simcan2.OperatingSystem.CpuSchedulers.CpuSchedulerRR.CpuSchedulerRR;
import simcan2.Architecture.Network.Stack.NetworkStack;
import simcan2.Applications.Base.AppsVector;

module ArduinoR4 extends EdgeNode
{
    parameters:
        staticAppAssignment = default(true);            // It has to do with the dependency of the CheckingHubModule
        maxApps = 1;                           // Maximum app instances allowed

        // Hardware specs
        numCpuCores = default(1);                        // The cores of the CPU equipped in this node
        cpuSpeed = default(36);						 // The speed (in MIPS) of each Core
        cpuTick @unit(s) = default(60s);				 // The interval between CPU ticks (in seconds)
        memorySize @unit(GB) = 32kB;
        diskSize @unit(GB) = 32kB;
        diskReadBandwidth @unit(Mbps) = 800Mbps;
        diskWriteBandwidth @unit(Mbps) = 800Mbps;
}
