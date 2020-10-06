#include "DataCenterManager.h"
#include "Management/utils/LogUtils.h"

Define_Module(DataCenterManager);

DataCenterManager::~DataCenterManager(){
}

void DataCenterManager::initialize(){

    int result;

    // Init super-class
    CloudManagerBase::initialize();

        // Get parameters from module
        showDataCenterConfig = par ("showDataCenterConfig");

        // Get gates
        inGate = gate ("in");
        outGate = gate ("out");

        // Parse data-center list
        result = parseDataCenterConfig();
        result = initDataCenterMetadata();

        // Something goes wrong...
        if (result == SC_ERROR){
            error ("Error while parsing data-centers list");
        }
        else if (showDataCenterConfig){
            EV_DEBUG << dataCenterToString ();
        }
}

int DataCenterManager::initDataCenterMetadata (){
    int result = SC_OK;
    DataCenter *pDataCenterBase = dataCentersBase[0];
    cModule *pRackModule, *pBoardModule, *pNodeModule;
    int numBoards, numNodes, numCores, numTotalCores = 0;

    for (int nRackIndex=0; nRackIndex < pDataCenterBase->getNumRacks(false); nRackIndex++ ) {
        Rack* pRackBase = pDataCenterBase->getRack(nRackIndex,false);
        //Generate rack name in the data center
        string strRackName = "rackCmp_" + pRackBase->getRackInfo()->getName() + "_" + std::to_string(nRackIndex);
        pRackModule = getParentModule()->getSubmodule(strRackName.c_str());

        numBoards =  pRackModule->par("numBoards");

        for (int nBoardIndex=0; nBoardIndex < numBoards; nBoardIndex++) {
            pBoardModule = pRackModule->getSubmodule("board", nBoardIndex);
            numNodes = pBoardModule->par("numBlades");
            numTotalCores+=numNodes;

            for (int nNodeIndex=0; nNodeIndex < numNodes; nNodeIndex++) {
                pNodeModule = pBoardModule->getSubmodule("blade", nNodeIndex);

                storeNodeMetadata(pNodeModule);

            }
        }


    }

    nTotalCores = nTotalAvailableCores = numTotalCores;

    return result;
}

int DataCenterManager::storeNodeMetadata(cModule *pNodeModule) {
    int result = SC_OK;
    cModule *pHypervisorModule;
    Hypervisor *pHypervisor;
    int numCores;

    pHypervisorModule = pNodeModule->getSubmodule("osModule")->getSubmodule("hypervisor");

    numCores = pNodeModule->par("numCpuCores");

    pHypervisor = check_and_cast<Hypervisor*>(pHypervisorModule);

    //Store hypervisor pointers by number of cores
    mapHypervisorPerNodes[numCores].push_back(pHypervisor);

    return result;
}


int DataCenterManager::parseDataCenterConfig (){
    int result;
    const char *dataCentersConfigChr;

    dataCentersConfigChr= par ("dataCenterConfig");
    DataCenterConfigParser dataCenterParser(dataCentersConfigChr);
    result = dataCenterParser.parse();
    if (result == SC_OK) {
        dataCentersBase = dataCenterParser.getResult();
    }
    return result;
}

void DataCenterManager::parseConfig(){
    CloudManagerBase::parseConfig();
    parseDataCenterConfig();
}


std::string DataCenterManager::dataCenterToString (){

    std::ostringstream info;
    int i;

        info << std::endl;

    return info.str();
}


cGate* DataCenterManager::getOutGate (cMessage *msg){

    cGate* nextGate;

        // Init...
        nextGate = nullptr;

        // If msg arrives from the Hypervisor
        if (msg->getArrivalGate()==inGate){
            nextGate = outGate;
        }

        // Msg arrives from an unknown gate
        else
            error ("Message received from an unknown gate [%s]", msg->getName());


    return nextGate;
}


void DataCenterManager::processSelfMessage (cMessage *msg){

}

void DataCenterManager::processRequestMessage (SIMCAN_Message *sm){
    if (sm->getOperation () == SM_VM_Req){
        handleVmRequestFits(sm);
    }

}

void DataCenterManager::handleVmRequestFits(SIMCAN_Message *sm)
{
    SM_UserVM *userVM_Rq;

    userVM_Rq = dynamic_cast<SM_UserVM*>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request"  << endl;

    if(userVM_Rq != nullptr)
      {
        userVM_Rq->printUserVM();
        //Check if is a VmRequest or a subscribe
        if (checkVmUserFit(userVM_Rq))
            EV_FATAL << "Ok" << endl;
            //acceptVmRequest(userVM_Rq);
        else
            //rejectVmRequest(userVM_Rq);
            EV_FATAL << "Fail" << endl;
      }
    else
      {
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());
      }
}

int DataCenterManager::getNTotalAvailableCores() const {
return nTotalAvailableCores;
}

void DataCenterManager::setNTotalAvailableCores(int nTotalAvailableCores) {
this->nTotalAvailableCores = nTotalAvailableCores;
}

int DataCenterManager::getNTotalCores() const {
return nTotalCores;
}

void DataCenterManager::setNTotalCores(int nTotalCores) {
this->nTotalCores = nTotalCores;
}

bool DataCenterManager::checkVmUserFit(SM_UserVM*& userVM_Rq)
{
    bool bRet,
         bAccepted;

    int nTotalRequestedCores,
        nRequestedVms,
        nAvailableCores,
        nTotalCores;

    std::string nodeIp,
                strUserName,
                strVmId;

    Hypervisor *hypervisor;

    bAccepted = bRet = true;
    if(userVM_Rq != nullptr)
      {
        nRequestedVms = userVM_Rq->getTotalVmsRequests();

        EV_DEBUG << "checkVmUserFit- Init" << endl;
        EV_DEBUG << "checkVmUserFit- checking for free space, " << nRequestedVms << " Vm(s) for the user" << userVM_Rq->getUserID() << endl;

        //Before starting the process, it is neccesary to check if the
        nTotalRequestedCores = calculateTotalCoresRequested(userVM_Rq);
        nAvailableCores = getNTotalAvailableCores();

        if(nTotalRequestedCores<=nAvailableCores)
          {
            nTotalCores = getNTotalCores();
            EV_DEBUG << "checkVmUserFit - There is available space: [" << userVM_Rq->getUserID() << nTotalRequestedCores<< " vs Available ["<< nAvailableCores << "/" <<nTotalCores << "]"<<endl;

            strUserName = userVM_Rq->getUserID();
            //Process all the VMs
            for(int i=0;i<nRequestedVms && bRet;i++)
              {
                EV_DEBUG << endl <<"checkVmUserFit - Trying to handle the VM: " << i << endl;

                //Get the VM request
                VM_Request& vmRequest = userVM_Rq->getVms(i);

//                //Create and fill the noderesource  with the VMrequest
//                NodeResourceRequest *pNode = generateNode(strUserName, vmRequest);
//
//                //Send the request to the DC
//                bAccepted = datacenterCollection->handleVmRequest(pNode);

                hypervisor = selectNode(vmRequest);

                //We need to know the price of the Node.
                //userVM_Rq->createResponse(i, bAccepted, pNode->getStartTime(), pNode->getIp(), pNode->getPrice());
                //bRet &= bAccepted;

//                if(!bRet)
//                  {
//                    clearVMReq (userVM_Rq, i);
//                    EV_DEBUG << "checkVmUserFit - The VM: " << i << "has not been handled, not enough space, all the request of the user " << strUserName << "have been deleted" << endl;
//                  }
//                else
//                  {
//                    //Getting VM and scheduling renting timeout
//                    vmRequest.pMsg = scheduleRentingTimeout(EXEC_VM_RENT_TIMEOUT, strUserName, vmRequest.strVmId, vmRequest.nRentTime_t2);
//
//                    //Update value
//                    nAvailableCores = datacenterCollection->getTotalAvailableCores();
//                    EV_DEBUG << "checkVmUserFit - The VM: " << i << " has been handled and stored sucessfully, available cores: "<< nAvailableCores << endl;
//                  }
              }
            //Update the data
            //nAvailableCores = datacenterCollection->getTotalAvailableCores();
            //nTotalCores = datacenterCollection->getTotalCores();

            EV_DEBUG << "checkVmUserFit - Updated space#: [" << userVM_Rq->getUserID() << "Requested: "<< nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << endl;
          }
        else
          {
            EV_DEBUG << "checkVmUserFit - There isnt enough space: [" << userVM_Rq->getUserID() << nTotalRequestedCores << " vs Available [" << nAvailableCores << "/" << nTotalCores << "]" << endl;
            bRet = false;
          }

        if(bRet)
            EV_DEBUG << "checkVmUserFit - Reserved space for: " << userVM_Rq->getUserID() << endl;
        else
            EV_DEBUG << "checkVmUserFit - Unable to reserve space for: " << userVM_Rq->getUserID() << endl;
      }
    else
      {
        EV_ERROR << "checkVmUserFit - WARNING!! nullpointer detected" <<endl;
        bRet = false;
      }

    EV_DEBUG << "checkVmUserFit- End" << endl;

    return bRet;
}


void DataCenterManager::processResponseMessage (SIMCAN_Message *sm){

}

Hypervisor* DataCenterManager::selectNode (const VM_Request& vmRequest){
    VirtualMachine *pVMBase;
    int numCoresRequested;
    std::map<int, std::vector<Hypervisor*>>::iterator it;

    pVMBase = findVirtualMachine(vmRequest.strVmType);
    numCoresRequested = pVMBase->getNumCores();

    for (it = mapHypervisorPerNodes.begin(); it != mapHypervisorPerNodes.end(); ++it){
        int key = it->first;
        EV_FATAL << key;
    }


}



int DataCenterManager::calculateTotalCoresRequested(SM_UserVM* userVM_Rq)
{
    int nRet, nRequestedVms;
    VM_Request vmRequest;

    nRet=nRequestedVms=0;
    if(userVM_Rq != NULL)
    {
        nRequestedVms = userVM_Rq->getTotalVmsRequests();

        for(int i=0;i<nRequestedVms;i++)
        {
            vmRequest = userVM_Rq->getVms(i);

            nRet+=getTotalCoresByVmType(vmRequest.strVmType);
        }
    }
    EV_DEBUG << "User:" << userVM_Rq->getUserID() << " has requested: "<< nRet << " cores" << endl;

    return nRet;
}

int DataCenterManager::getTotalCoresByVmType(std::string strVmType)
{
    int nRet;
    VirtualMachine* pVmType;

    nRet=0;

    pVmType = findVirtualMachine(strVmType);

    if(pVmType != NULL)
    {
        nRet = pVmType->getNumCores();
    }

    return nRet;
}
