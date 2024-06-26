package s2f.Architecture.Nodes.Iot;

import s2f.Architecture.Nodes.HardwareManagers.HardwareManager.HardwareManager;
import s2f.Architecture.Cpu.CpuProcessor.CpuProcessor;
import s2f.OperatingSystem.Hypervisors.EdgeHypervisor.EdgeHypervisor;
import s2f.OperatingSystem.CpuSchedulers.ICpuScheduler;
import s2f.OperatingSystem.CpuSchedulers.CpuSchedulerRR.CpuSchedulerRR;
import s2f.Architecture.Network.Stack.NetworkStack;
import s2f.Applications.Base.AppsVector;

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
