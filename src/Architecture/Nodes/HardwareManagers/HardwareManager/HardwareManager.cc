#include "HardwareManager.h"

Define_Module(HardwareManager);

void HardwareManager::initialize(){

    int i;

        // Obtain the parameters of the module
        isVirtualHardware = par ("isVirtualHardware");
        maxVMs = par ("maxVMs");
        numCpuCores = par ("numCpuCores");
        maxUsers = par ("maxUsers");

        // Check consistency (cloud environments)
        if (isVirtualHardware){

            if ((maxVMs<=0) || (maxVMs>numCpuCores))
                error ("For virtual environments, 0 < maxVMs <= numCpuCores");

            if (numCpuCores <= 0)
                error ("The parameter numCpuCores must be a positive value (>0)");

            if (maxUsers != maxVMs)
                error ("For virtual environments, maxUsers = maxVMs");
        }
        // Check consistency (cluster environments)
        else {
            if (maxVMs != 1)
                error ("For non-virtual environments, maxVMs must be 1");

            if (numCpuCores <= 0)
                error ("The parameter numCpuCores must be a positive value (>0)");

            if (maxUsers <= 0)
                error ("The parameter maxUsers must be a positive value (>0)");
        }



//        // Create user vector
//        userVector = new UserExecution* [maxVMs];
//
//        // Init user vector
//        for (i=0; i<maxVMs; i++)
//            userVector[i] = nullptr;
//




//        // Create aux structures for CPU schedulers
//        if (isVirtualHardware){
//
//            cpuSchedulerUtilization = new bool[numCpuCores];
//
//            for (i=0; i<numCpuCores; i++)
//               cpuSchedulerUtilization[i] = false;
//    }
//        else{
//            cpuSchedulerUtilization = nullptr;
//        }

}

void HardwareManager::handleMessage(cMessage *msg){
    // TODO - Generated method body
}
