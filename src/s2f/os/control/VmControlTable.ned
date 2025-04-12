package s2f.os.control;

module VmControlTable
{
    parameters:
        @class(VmControlTable);
        int numVMs;
        int numAppsPerVm;
        @display("i=block/table,,0;bgb=,,,,0");
}
