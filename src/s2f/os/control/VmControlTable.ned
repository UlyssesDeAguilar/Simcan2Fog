package s2f.os.control;

//
// Module that holds all VM and application context
//
// @author Ulysses de Aguilar Gudmundsson
// @version 1.0
simple VmControlTable
{
    parameters:
        int numVMs; // Number of supported vms on the node
        int numAppsPerVm; // Number of apps per vm
        @display("i=block/table,,0;bgb=,,,,0");
}
