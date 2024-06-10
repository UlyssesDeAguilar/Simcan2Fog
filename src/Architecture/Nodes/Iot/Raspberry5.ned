package simcan2.Architecture.Nodes.Iot;

import simcan2.Architecture.Nodes.HardwareManagers.HardwareManager.HardwareManager;
import simcan2.Architecture.Cpu.CpuProcessor.CpuProcessor;
import simcan2.OperatingSystem.Hypervisors.EdgeHypervisor.EdgeHypervisor;
import simcan2.OperatingSystem.CpuSchedulers.ICpuScheduler;
import simcan2.OperatingSystem.CpuSchedulers.CpuSchedulerRR.CpuSchedulerRR;
import simcan2.Architecture.Network.Stack.NetworkStack;
import simcan2.Applications.Base.AppsVector;

module Raspberry5 extends EdgeNode
{
    parameters:
        @display("i=pi64");
        staticAppAssignment = default(true);   // It has to do with the dependency of the CheckingHubModule
        maxApps = 5;                           // Maximum app instances allowed

        // Hardware specs
        numCpuCores = 4;                       // The cores of the CPU equipped in this node
        cpuSpeed = default(20558);			   // The speed (in MIPS) of each Core
        cpuTick @unit(s) = default(60s);	   // The interval between CPU ticks (in seconds)
        memorySize @unit(GB) = 4GB;
        diskSize @unit(GB) = 8GB;
        diskReadBandwidth @unit(Mbps) = 800Mbps;
        diskWriteBandwidth @unit(Mbps) = 800Mbps;
}
