#include "Hypervisor.h"

Define_Module (Hypervisor);


Hypervisor::~Hypervisor(){
    mapVmScheduler.clear();
    mapResourceRequestPerVm.clear();
}

void Hypervisor::initialize(){

	int i, numCpuGates, numAppGates;

	    // Init the super-class
	    cSIMCAN_Core::initialize();

        // Init module parameters
        isVirtualHardware = par ("isVirtualHardware");
        maxVMs = (unsigned int) par ("maxVMs");
//            numAllocatedVms = 0;

        // Get the number of gates for each vector
        numAppGates = gateSize ("fromApps");
        numCpuGates = gateSize ("fromCpuScheduler");

        // Init the size of the cGate vectors
        fromAppsGates = new cGate* [numAppGates];
        toAppsGates = new cGate* [numAppGates];

        // Init the cGates vector for Applications
        for (i=0; i<numAppGates; i++){
            fromAppsGates [i] = gate ("fromApps", i);
            toAppsGates [i] = gate ("toApps", i);

            // Checking connections
            if (!toAppsGates[i]->isConnected()){
                EV_ERROR << "toAppsGates[" << i << "] is not connected";
                error ("toAppsGates is not connected");
            }
        }

        // Init the cGates vector for CPU scheduler
        fromCPUGates = new cGate* [numCpuGates];
        toCPUGates = new cGate* [numCpuGates];

        for (i=0; i<numCpuGates; i++){
            fromCPUGates [i] = gate ("fromCpuScheduler", i);
            toCPUGates [i] = gate ("toCpuScheduler", i);
        }

        pAppsVectors = getParentModule()->getParentModule()->getSubmodule("appsVectors", 0);
        pAppsVectorsArray = new cModule* [pAppsVectors->getVectorSize()];

        for (int i=0; i<pAppsVectors->getVectorSize(); i++)
            pAppsVectorsArray[i] = getParentModule()->getParentModule()->getSubmodule("appsVectors", i);

        pCpuScheds = getModuleByPath("^.cpuSchedVector")->getSubmodule("cpuScheduler", 0);
        pCpuSchedArray = new cModule* [pCpuScheds->getVectorSize()];
        freeSchedArray = new bool [pCpuScheds->getVectorSize()];
        for (int i=0; i<pCpuScheds->getVectorSize(); i++)
        {
            pCpuSchedArray[i] = getModuleByPath("^.cpuSchedVector")->getSubmodule("cpuScheduler", i);
            freeSchedArray[i] = true;
        }

        pHardwareManagerModule = getModuleByPath("^.^.hardwareManager");;
        pHardwareManager = check_and_cast<HardwareManager*>(pHardwareManagerModule);

        if (pHardwareManager == nullptr)
            error ("HardwareManager not found!");
}


void Hypervisor::finish(){

	// Finish the super-class
	cSIMCAN_Core::finish();
}


cGate* Hypervisor::getOutGate (cMessage *msg){

    cGate* outGate;

           // Init...
           outGate = nullptr;

            // If msg arrives from Application gates
            if (msg->arrivedOn("fromApps")){
               outGate = gate ("toApps", msg->getArrivalGate()->getIndex());
            }

            // If msg arrives from CPU scheduler
            else if (msg->arrivedOn("fromCpuScheduler")){
                error ("This module cannot receive requests from the CPU system!");
            }

            // Msg arrives from an unknown gate
            else
                error ("Message received from an unknown gate [%s]", msg->getName());

    return outGate;
}


void Hypervisor::processSelfMessage (cMessage *msg){
    error ("This module cannot process self messages:%s", msg->getName());
}


void Hypervisor::processRequestMessage (SIMCAN_Message *sm){

    // Message came from CPU
    if (sm->arrivedOn("fromCpuScheduler")){
        error ("This module cannot receive request messages from CPU!!!");
    }

    // Message came from applications
    else if (sm->arrivedOn("fromApps")){

        EV_DEBUG << "(processRequestMessage) Message arrives from applications."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

        // CPU operation?
        if (sm->getOperation () == SM_ExecCpu || sm->getOperation () == SM_AbortCpu ){

            // Virtual HW! There is only 1 CPU scheduler
            if (!isVirtualHardware){

                // Debug
                EV_INFO << "Sending request message to CPU."<< endl << sm->contentsToString(showMessageContents, showMessageTrace).c_str() << endl;

                sendRequestMessage (sm, toCPUGates[0]);
            }
            else{
                //TODO: Manage users/VMs to redirec requests to the corresponding CPU scheduler

                // Debug
                EV_INFO << "Sending request message to CPU."<< endl << sm->contentsToString(showMessageContents, showMessageTrace).c_str() << endl;

                // Checks for the next module
                if (sm->getNextModuleIndex() == SM_UnsetValue){
                    error ("Unset value for nextModuleIndex... and there are several output gates. %s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
                }

                // Next module index is out of bounds...
                else if ((sm->getNextModuleIndex() < 0) || (sm->getNextModuleIndex() >= gateSize("toCpuScheduler"))){
                    error ("nextModuleIndex %d is out of bounds [%d]", sm->getNextModuleIndex(), gateSize("toCpuScheduler"));
                }

                // Everything is OK... send the message! ;)
                else{

                    // Debug (Debug)
                    EV_DEBUG << "(processRequestMessage) Sending request message. There are several output gates -> toOutputGates["<< sm->getNextModuleIndex() << "]" << endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;

                    sendRequestMessage (sm, toCPUGates[sm->getNextModuleIndex()]);
                }
            }
        }

        // Unknown operation!
        else{
            error ("Unknown operation:%d", sm->getOperation ());
        }


//        else
//              // I/O operation?
//              if ((sm->getOperation () == SM_OPEN_FILE)   ||
//                  (sm->getOperation () == SM_CLOSE_FILE)  ||
//                  (sm->getOperation () == SM_READ_FILE)   ||
//                  (sm->getOperation () == SM_WRITE_FILE)  ||
//                  (sm->getOperation () == SM_CREATE_FILE) ||
//                  (sm->getOperation () == SM_DELETE_FILE)){
//
//                      // Remote operation? to NET
//                      if (sm->getRemoteOperation()){
//
//        //                    if (DEBUG_Service_Redirector)
//        //                        showDebugMessage ("Local request redirected to NET. %s",
//        //                                            sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//                          sendRequestMessage (sm, toNetGate);
//                      }
//
//                      // Local operation? to local FS
//                      else{
//
//        //                    if (DEBUG_Service_Redirector)
//        //                        showDebugMessage ("Local request redirected to I/O. %s",
//        //                                            sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//        //
//                          sendRequestMessage (sm, toMemoryGate);
//                      }
//              }
//
//              // MPI operation?
//              else if ((sm->getOperation () == MPI_SEND) ||
//                       (sm->getOperation () == MPI_RECV) ||
//                       (sm->getOperation () == MPI_BARRIER_UP)   ||
//                       (sm->getOperation () == MPI_BARRIER_DOWN) ||
//                       (sm->getOperation () == MPI_BCAST)   ||
//                       (sm->getOperation () == MPI_SCATTER) ||
//                       (sm->getOperation () == MPI_GATHER)){
//
//                  sendRequestMessage (sm, toNetGate);
//              }
//
//
//
//
//              // MEM operation?
//              else if ((sm->getOperation () == SM_MEM_ALLOCATE) ||
//                       (sm->getOperation () == SM_MEM_RELEASE)){
//
//        //            if (DEBUG_Service_Redirector)
//        //                showDebugMessage ("Local request redirected to MEM. %s", sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//                  sendRequestMessage (sm, toMemoryGate);
//              }
//
//
//              // Net operation?
//              else if ((sm->getOperation () == SM_CREATE_CONNECTION) ||
//                      (sm->getOperation () == SM_LISTEN_CONNECTION) ||
//                      (sm->getOperation () == SM_SEND_DATA_NET)){
//
//        //            if (DEBUG_Service_Redirector)
//        //                showDebugMessage ("Local request redirected to NET. %s", sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//                  sendRequestMessage (sm, toNetGate);
//              }

    }



//	// Msg cames from Network
//	if (sm->getArrivalGate() == fromNetGate){
//
//		if (DEBUG_Service_Redirector)
//			showDebugMessage ("Incomming request from NET... sending to App[%d]. %s",
//								sm->getNextModuleIndex(),
//								sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//		sendRequestMessage (sm, toAppGates[sm->getNextModuleIndex()]);
//	}

	
	
	
	
//	// Msg cames from Memory
//	else if (sm->getArrivalGate() == fromMemoryGate){
//
//		if (DEBUG_Service_Redirector)
//			showDebugMessage ("Incomming request from OS... sending to App[%d]. %s",
//								sm->getNextModuleIndex(),
//								sm->contentsToString(DEBUG_MSG_Service_Redirector).c_str());
//
//		sendRequestMessage (sm, toAppGates[sm->getNextModuleIndex()]);
//	}

}


void Hypervisor::processResponseMessage (SIMCAN_Message *sm){

    // Debug (Debug)
    EV_DEBUG << "(processResponseMessage) Sending response message."<< endl << sm->contentsToString(showMessageContents, showMessageTrace) << endl;
		
	// Send back the message
	sendResponseMessage (sm);
}

int Hypervisor::getAvailableCores() {
    return pHardwareManager->getAvailableCores();
}

cModule* Hypervisor::allocateNewResources(NodeResourceRequest* pResourceRequest) {
    unsigned int* cpuCoreIndex;
    cpuCoreIndex = pHardwareManager->allocateCores(pResourceRequest->getTotalCpus());

    if (cpuCoreIndex == nullptr)
        return nullptr;

    if (!pHardwareManager->allocateRam(pResourceRequest->getTotalMemory()))
    {
        pHardwareManager->deallocateCores(pResourceRequest->getTotalCpus(), cpuCoreIndex);
        return nullptr;
    }

    if (!pHardwareManager->allocateDisk(pResourceRequest->getTotalDiskGb()))
    {
        pHardwareManager->deallocateCores(pResourceRequest->getTotalCpus(), cpuCoreIndex);
        pHardwareManager->deallocateRam(pResourceRequest->getTotalMemory());
        return nullptr;
    }

    bool allocatedVm = false;
    int nSchedulerIndex = -1;

    for (int i=0; i<maxVMs && !allocatedVm; i++) {
        if (freeSchedArray[i]) {
            nSchedulerIndex = i;
            freeSchedArray[nSchedulerIndex] = false;
            allocatedVm = true;
        }
    }

    if (!allocatedVm)
        return nullptr;

    cModule *pVmSchedulerModule = pCpuSchedArray[nSchedulerIndex];
    cModule *pVmAppVectorModule = pAppsVectorsArray[nSchedulerIndex];

    mapResourceRequestPerVm[pResourceRequest->getVmId()] = pResourceRequest;
    mapVmScheduler[pResourceRequest->getVmId()] = nSchedulerIndex;

    CpuSchedulerRR *pVmScheduler = check_and_cast<CpuSchedulerRR*> (pVmSchedulerModule);
    int nManagedCpuCores = pResourceRequest->getTotalCpus();

    bool* isCPU_Idle = new bool [nManagedCpuCores];
    for (int i=0; i<nManagedCpuCores; i++)
        isCPU_Idle[i] = true;

    pVmScheduler->setManagedCpuCores(nManagedCpuCores);
    pVmScheduler->setCpuCoreIndex(cpuCoreIndex);
    pVmScheduler->setIsCpuIdle(isCPU_Idle);
    pVmScheduler->setIsRunning(true);

    return pVmAppVectorModule;
}

void Hypervisor::deallocateVmResources(std::string strVmId) {
    std::map<std::string, NodeResourceRequest*>::iterator requestIt;
    NodeResourceRequest* pResourceRequest;
    std::map<string, int>::iterator schedulerIt;
    int nSchedulerIndex;


    requestIt = mapResourceRequestPerVm.find(strVmId);
    schedulerIt = mapVmScheduler.find(strVmId);

    if (requestIt==mapResourceRequestPerVm.end() || schedulerIt==mapVmScheduler.end())
        return;

    pResourceRequest = requestIt->second;
    nSchedulerIndex = schedulerIt->second;



    cModule *pVmSchedulerModule = pCpuSchedArray[nSchedulerIndex];
    CpuSchedulerRR *pVmScheduler = check_and_cast<CpuSchedulerRR*> (pVmSchedulerModule);

    unsigned int* cpuCoreIndex;
    cpuCoreIndex = pVmScheduler->getCpuCoreIndex();
    pHardwareManager->deallocateCores(pResourceRequest->getTotalCpus(), cpuCoreIndex);
    freeSchedArray[nSchedulerIndex] = true;
    pHardwareManager->deallocateRam(pResourceRequest->getTotalMemory());

    pVmScheduler->setIsRunning(false);
}

bool* Hypervisor::getFreeCoresArrayPtr() const {
    return pHardwareManager->getFreeCoresArrayPtr();
}


//int Hypervisor::executeApp(Application* appType)
//{
//    int nInputDataSize, nOutputDataSize, nMIs, nIterations, nTotalTime;
//    int nReadTime, nWriteTime, nProcTime;
//    AppParameter* paramInputDataSize, *paramOutputDataSize, *paramMIs, *paramIterations;
//
//    nTotalTime = nInputDataSize = nOutputDataSize = nMIs = nIterations = nTotalTime = 0;
//
//    //TODO: Cuidado con esto a ver si no peta.
//    //Esto es un apaño temporal para no ejecutarlo en los datacenters reales
//    if(appType!=NULL && appType->getAppName().compare("AppDataIntensive")==0)
//    {
//        //DatasetInput
//        paramInputDataSize = appType->getParameterByName("inputDataSize");
//        paramOutputDataSize = appType->getParameterByName("outputDataSize");
//        paramMIs = appType->getParameterByName("MIs");
//        paramIterations = appType->getParameterByName("iterations");
//
//        if(paramInputDataSize != nullptr)
//        {
//            nInputDataSize = std::stoi(paramInputDataSize->getValue());
//        }
//        if(paramOutputDataSize != nullptr)
//        {
//            nOutputDataSize = std::stoi(paramOutputDataSize->getValue());
//        }
//        if(paramMIs != nullptr)
//        {
//            nMIs = std::stoi(paramMIs->getValue());
//        }
//        if(paramIterations != nullptr)
//        {
//            nIterations = std::stoi(paramIterations->getValue());
//        }
//
////        getParentModule()->getParentModule()->getSubmodule('appsVectors', 0);
//
//        cModuleType *moduleType = cModuleType::get(appType->getType().c_str());
//
//
////        cModule *moduleApp = moduleType->create(appType->getAppName(), , , );
//
//
////        //TODO: Esto está hecho a fuego con 1 sola app. Diversificar.
////        //Leer 10 GB (transmision por red+carga en disco)
////        nReadTime = nInputDataSize*1024/SPEED_R_DISK;
////
////        //Escribir Gb (escritura en disco y envío por red);
////        nWriteTime = nOutputDataSize*1024/SPEED_W_DISK;
////
////        //Procesar 10GB
////        nProcTime = nMIs/CPU_SPEED;
////
////        nTotalTime = (nReadTime+nWriteTime+nProcTime)*nIterations;
////
////        EV_INFO << "The total executing, R: " << nReadTime << " | W: " << nWriteTime << " | P: " << nProcTime << " | Total: " << nTotalTime<< endl;
//    }
//    else if(appType!=NULL && appType->getAppName().compare("otraApp"))
//    {
//
//    }
//
//    return nTotalTime;
//}
