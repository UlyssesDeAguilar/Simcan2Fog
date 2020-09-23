#include "CloudProviderFirstFitSingleUser.h"

Define_Module(CloudProviderFirstFitSingleUser);

#define CPU_SPEED       300000
#define SPEED_W_DISK    120
#define SPEED_R_DISK    250

#define INITIAL_STAGE  "INITIAL_STAGE"
#define EXEC_APP_END  "EXEC_APP_END"
#define MANAGE_SUBSCRIBTIONS  "MANAGE_SUBSCRIBTIONS"
#define SIMCAN_MESSAGE "SIMCAN_Message"

CloudProviderFirstFitSingleUser::~CloudProviderFirstFitSingleUser(){
}


void CloudProviderFirstFitSingleUser::initialize(){

    // Init super-class
    CloudProviderBase::initialize();
    loadNodes();

    bFinished = false;
    scheduleAt(simTime().dbl()+1, new cMessage(INITIAL_STAGE));
}

void CloudProviderFirstFitSingleUser::processSelfMessage (cMessage *msg){

    std::string strUsername;
    SM_UserAPP *userAPP_Rq;
    int nVmIndex;

    // Start execution?
    if(!strcmp(msg->getName(), INITIAL_STAGE))
    {

        EV_INFO << "processSelfMessage - INITIAL_STAGE" << endl;
        scheduleAt(simTime().dbl()+1, new cMessage(MANAGE_SUBSCRIBTIONS));
        delete (msg);
    }
    else if (!strcmp(msg->getName(), EXEC_APP_END)){

        EV_INFO << "processSelfMessage - EXEC_APP_END" << endl;
        userAPP_Rq = dynamic_cast<SM_UserAPP *>(msg);

        if(userAPP_Rq != nullptr)
        {
            strUsername = userAPP_Rq->getUserID();
            userAPP_Rq->increaseFinishedApps();
            EV_INFO << "The execution of App(s) launched by the user "<< strUsername << " have finished" << endl;

            freeUserVms(strUsername);

            //Check the subscription queue
            updateSubsQueue();

            //Notify the user the end of the execution
            acceptAppRequest(userAPP_Rq);

        }
    }
    else if(!strcmp(msg->getName(), MANAGE_SUBSCRIBTIONS))
    {
        bool bFreeSpace;
        SM_UserVM* userVmSub;
        bFreeSpace = false;
        //EV_INFO << "processSelfMessage - MANAGE_SUBSCRIBTIONS" << endl;

        if(subscribeQueue.size()>0)
        {
            EV_INFO << "The queue of subscription: " << subscribeQueue.size() << endl;

            //First of all, check the subscriptions timeouts
            for(int i=0;i<subscribeQueue.size();i++)
            {
                userVmSub = subscribeQueue.at(i);

                if(userVmSub  != nullptr)
                {
                    strUsername = userVmSub->getUserID();

                    if(simTime().dbl() > userVmSub->getMaxSubscribetime(0))
                    {
                        EV_INFO << "The subscription of the user:  " << strUsername << " has expired, TIMEOUT!" << userVmSub->getMaxStartTime(0) << " vs " << simTime().dbl() << endl;
                        freeUserVms(strUsername);
                        bFreeSpace = true;
                        subscribeQueue.erase(subscribeQueue.begin()+i);
                        i--;

                        //send the Timeout
                        timeoutSubscription(userVmSub);
                    }
                }
            }
            //Check the subscription queue
            updateSubsQueue();

        }

        delete (msg);
        if(!bFinished && simTime().dbl()<30000000)
            scheduleAt(simTime().dbl()+1, new cMessage(MANAGE_SUBSCRIBTIONS));


    }else
      error ("Unknown self message [%s]", msg->getName());
}
void CloudProviderFirstFitSingleUser::updateSubsQueue()
{
    SM_UserVM* userVmSub;

    //Finally, notify if there is enough space to handle a new notification
    for(int i=0;i<subscribeQueue.size();i++)
    {
        userVmSub = subscribeQueue.at(i);

        if(checkVmUserFit(userVmSub))
        {
            EV_INFO << "Notifying subscription of user: "<< userVmSub->getUserID()<<endl;
            notifySubscription(userVmSub);

            //Remove from queue
            subscribeQueue.erase(subscribeQueue.begin()+i);
            i--;
        }
    }
}
void CloudProviderFirstFitSingleUser::freeUserVms(std::string strUsername)
{
    int nVmIndex;
    NodeUser* nodeUser;
    VM_HashSearch vmHashSearch;

    //Mark the user VMs as free
    if(userRqMap.find(strUsername) != userRqMap.end())
    {
        vmHashSearch = userRqMap.at(strUsername);
        for(int i=0;i<vmHashSearch.vmList.size();i++)
        {
            nVmIndex = vmHashSearch.vmList.at(i);
            nodeUser = nodeUsers.at(nVmIndex);
            if(nodeUser != nullptr)
            {
              nodeUser->setReserved(false);
              nodeUser->setAvailableCpUs(nodeUser->getTotalCpUs());
              nFreeNodes++;
            }
        }
    }

}
void CloudProviderFirstFitSingleUser::loadNodes()
{
    DataCenter* pDataCenter;
    Rack* pRack;
    Node* pNode;
    NodeUser* pNodeUser;
    int nComputingRacks, nTotalNodes, nNumCpus;

    //Go over the datacenter object: all racks
    if(dataCentersBase.size()>0)
    {

        pDataCenter = dataCentersBase.at(0);
        if(pDataCenter != nullptr)
        {
            nComputingRacks = pDataCenter->getNumRacks(false);
            for(int i=0;i<nComputingRacks; i++)
            {
                EV_INFO << "Adding computing rack: "<< i << endl;
                pRack = pDataCenter->getRack(i, false);
                nTotalNodes = pRack->getNumNodes();

                //Create the numnodes
                for(int j=0;j<nTotalNodes;j++)
                {
                    //We get the node, and check it
                    pNode = pRack->getNode(j);

                    if(pNode != nullptr)
                    {
                        //EV_INFO << "Adding computing node : "<< j << " | rack: " << i << endl;
                        nNumCpus = pNode->getNumAvailableCpus();
                        pNodeUser = new NodeUser();
                        pNodeUser->setReserved(false);
                        pNodeUser->setTotalCpUs(nNumCpus);
                        pNodeUser->setAvailableAllCpus();
                        pNodeUser->setIp(std::to_string(nFreeNodes));
                        nodeUsers.push_back(pNodeUser);
                        nFreeNodes++;
                    }
                }
            }
        }
    }

}
void CloudProviderFirstFitSingleUser::processRequestMessage (SIMCAN_Message *sm){

    SM_UserVM *userVM_Rq;
    SM_UserAPP *userAPP_Rq;
    SM_CloudProvider_Control* userControl;
    int nElements;

    userVM_Rq = dynamic_cast<SM_UserVM *>(sm);
    userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);
    //userControl = dynamic_cast<SM_CloudProvider_Control *>(sm);

    //Request message from user generator.
    if(userVM_Rq != nullptr)
    {
        userVM_Rq->printUserVM();

        EV_INFO << "Handle VMrequest"  << endl;

        //Handle a VmRequest
        if(userVM_Rq->getOperation() == SM_VM_Req)
        {
            //Check if is a VmRequest or a subscribe
            if(checkVmUserFit(userVM_Rq))
            {
                acceptVmRequest(userVM_Rq);
            }
            else
                rejectVmRequest(userVM_Rq);
        }
        //Handle a Subscribe
        else if(userVM_Rq->getOperation() == SM_VM_Sub)
        {
            //Subscribe request
            EV_INFO << "Received Subscribe operation"  << endl;
            if(checkVmUserFit(userVM_Rq))
            {
                notifySubscription(userVM_Rq);
            }
            else
            {
                //Store the vmRequest
                storeVmSubscribe(userVM_Rq);
            }
        }
    }
    else if(userAPP_Rq != nullptr)
    {
        EV_INFO << "Handle AppRequest"  << endl;

        //APPRequest
        userAPP_Rq->printUserAPP();

        //Perform the operations ...
        handleUserAppRequest(userAPP_Rq);

    }else if(userControl != nullptr)
    {
        EV_INFO << "Received end of party"  << endl;
        //Stop the checking process.
        bFinished = true;
    }
    else
    {
        EV_INFO << "Unknown message:"  << sm->getName() << endl;
    }
}
void CloudProviderFirstFitSingleUser::handleUserAppRequest(SM_UserAPP* userAPP_Rq)
{
    //Get the user name, and recover the info about the VmRequests;
    bool bHandle;
    std::string strUsername, strAppType;
    Application* appType;
    NodeUser* vmNodeUser;
    VM_HashSearch vmHashSearch;
    APP_Request userApp;
    int nInputDataSize, nOutputDataSize, nMIs, nIterations, nNodeIndex, nTotalTime;
    int nReadTime, nWriteTime, nProcTime;

    AppParameter* paramInputDataSize, *paramOutputDataSize, *paramMIs, *paramIterations, *paramNodeIndex, *paramTotalTime;

    bHandle = false;
    if(userAPP_Rq != nullptr)
    {
        strUsername = userAPP_Rq->getUserID();
        if(userRqMap.find(strUsername) != userRqMap.end())
        {
            vmHashSearch = userRqMap.at(strUsername);
            EV_INFO << "Executing the VMs corresponding with the user: " << strUsername << " | Total: "<< vmHashSearch.vmList.size() << endl;

            //First step consists in calculating the total units of time spent in executing the application.
            if(userAPP_Rq->getArrayAppsSize()>0)
            {
                //Get the app
                userApp =userAPP_Rq->getApp(0);
                strAppType = userApp.strApp;

                appType = appTypes.at(0);
                if(appType != nullptr)
                {
                    EV_INFO << 1 << endl;
                    //Get the parameters
                    if( strAppType.compare(appType->getAppName()))
                    {
                        //DatasetInput
                        paramInputDataSize = appType->getParameterByName("inputDataSize");
                        paramOutputDataSize = appType->getParameterByName("outputDataSize");
                        paramMIs = appType->getParameterByName("MIs");
                        paramIterations = appType->getParameterByName("iterations");

                        if(paramInputDataSize != nullptr)
                        {
                            nInputDataSize = std::stoi(paramInputDataSize->getValue());
                        }
                        if(paramOutputDataSize != nullptr)
                        {
                            nOutputDataSize = std::stoi(paramOutputDataSize->getValue());
                        }
                        if(paramMIs != nullptr)
                        {
                            nMIs = std::stoi(paramMIs->getValue());
                        }
                        if(paramIterations != nullptr)
                        {
                            nIterations = std::stoi(paramIterations->getValue());
                        }


                        //TODO: En este punto deberíamos hacer un scheduleAt por cada VM. En este caso solo cogemos la primera.
                        //for(...)
                        nNodeIndex = vmHashSearch.vmList.at(0);
                        vmNodeUser = nodeUsers.at(nNodeIndex);
                        if(nNodeIndex != -1)
                        {
                            //Leer 10 GB (transmision por red+carga en disco)
                            nReadTime = nInputDataSize*1024/SPEED_R_DISK;
                            //Escribir Gb (escritura en disco y envío por red);
                            nWriteTime = nOutputDataSize*1024/SPEED_W_DISK;
                            //Procesar 10GB
                            nProcTime = nMIs/CPU_SPEED;

                            nTotalTime = (nReadTime+nWriteTime+nProcTime)*nIterations;

                            EV_INFO << "The total executing, R: " << nReadTime << " | W: " << nWriteTime << " | P: " << nProcTime << " | Total: " << nTotalTime<< endl;
                        }
                        EV_INFO << "Msg name " << userAPP_Rq->getName() << endl;
                        //Schedule At de
                        userAPP_Rq->setName(EXEC_APP_END);

                        EV_INFO << "Msg name " << userAPP_Rq->getName() << endl;
                        scheduleAt(simTime().dbl()+nTotalTime, userAPP_Rq);
                        //vmNodeUser->setAppRequestMsg(userAPP_Rq);
                        bHandle = true;
                        //endfor
                    }
                }
                else
                {
                    EV_INFO << "WARNING! Invalid App type in user: " << userAPP_Rq->getName() << endl;
                }
            }
            else
            {
            EV_INFO << "WARNING! The App list is empty: " << userAPP_Rq->getName() << endl;
        }
        //TODO: else salir por error
    }
    //TODO: else salir por error
    }

    if(!bHandle)
    {
        userAPP_Rq->setResult(0);
        rejectAppRequest(userAPP_Rq);
    }

}
void CloudProviderFirstFitSingleUser::notifySubscription(SM_UserVM* userVM_Rq)
{
    EV_INFO << "Notifying request from user: " << userVM_Rq->getUserID() << endl;
    EV_INFO << "Last id gate: " <<userVM_Rq->getLastGateId()<< endl;

    //Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Accept);

    //Send the values
    sendResponseMessage(userVM_Rq);
}
void CloudProviderFirstFitSingleUser::timeoutSubscription(SM_UserVM* userVM_Rq)
{
    EV_INFO << "Notifying request from user:" << userVM_Rq->getUserID() << endl;
    EV_INFO << "eeaeae" << endl;
    EV_INFO << "Last id gate: " << userVM_Rq->getLastGateId() << endl;

    //Fill the message
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Notify);
    userVM_Rq->setResult(SM_APP_Sub_Timeout);

    //Send the values
    sendResponseMessage(userVM_Rq);
}
void CloudProviderFirstFitSingleUser::storeVmSubscribe(SM_UserVM* userVM_Rq)
{
    EV_INFO << "Subscribing the VM request from user:" << userVM_Rq->getUserID() << endl;

    //Store the VM subscription until there exist the Vms necessaries
    subscribeQueue.push_back(userVM_Rq);
}
void CloudProviderFirstFitSingleUser::processResponseMessage (SIMCAN_Message *sm){

}

bool CloudProviderFirstFitSingleUser::checkVmUserFit(SM_UserVM*& userVM_Rq)
{
    bool bRet, bReloadVmRequest;
    int nArraySize, nPrice, nRequestedNodes, nAcceptedNodes, nIndex, nVmNumber;
    std::string nodeIp, strUserName;
    VM_Request vmRequest;
    VM_HashSearch vmHashSearch;

    bRet = false;
    bReloadVmRequest = true;

    nIndex = nAcceptedNodes = 0;
    nPrice = 10;
    nRequestedNodes = userVM_Rq->getTotalVmsRequests();

    EV_INFO << "checkVmUserFit- Init" << endl;
    EV_INFO << "checkVmUserFit- checking for free space, " << nRequestedNodes << " Vm(s) for the user" << userVM_Rq->getUserID() << endl;

    //Tener una cuenta atómica de las maquinas disponibles
    if(nFreeNodes>nRequestedNodes)
    {
        vmRequest = userVM_Rq->getVms(0);
        strUserName = userVM_Rq->getUserID();
        vmHashSearch.strUser = strUserName;

        //Chequear que esten vacias y tirar palante.
        while((nAcceptedNodes<nRequestedNodes) && nIndex<nodeUsers.size())
        {
            if(bReloadVmRequest)
            {
                vmRequest = userVM_Rq->getVms(nAcceptedNodes);
                bReloadVmRequest= false;
            }
            NodeUser* pNode = nodeUsers.at(nIndex);
            if(pNode != nullptr)
            {
                if(!pNode->isReserved() && pNode->getTotalCpUs()==pNode->getAvailableCpUs())
                {
                    //The node is free.
                    //TODO: Hay que sacar el tipo de maquina, para reservar adecuadamente los cores.
                    nodeIp = pNode->getIp();
                    pNode->setAvailableCpUs(0);
                    pNode->setReserved(true);
                    pNode->setUserName(strUserName);
                    pNode->setMaxStartTimeT1(vmRequest.maxStartTime_t1);
                    pNode->setRentTimeT2(vmRequest.nRentTime_t2);
                    pNode->setMaxSubTimeT3(vmRequest.maxSubTime_t3);
                    pNode->setMaxSubscriptionTimeT4(vmRequest.maxSubscriptionTime_t4);
                    bReloadVmRequest=true;

                    nPrice = getPriceByVmType(vmRequest.strVmType);

                    //We need to know the price of the Node.
                    userVM_Rq->createResponse(nAcceptedNodes, true, 10+nAcceptedNodes, nodeIp, nPrice);
                    nAcceptedNodes++;
                    nFreeNodes--;
                    nVmNumber = std::stoi(nodeIp);

                    if(nVmNumber != -1)
                    {
                        vmHashSearch.vmList.push_back(nVmNumber);
                        EV_INFO << "checkVmUserFit - Selected node: " << nodeIp << endl;
                        bRet = true;
                    }
                    else
                    {
                        EV_INFO << "checkVmUserFit - Unable to select node!!!!" << nodeIp << endl;
                    }
                }
                else
                {
                    EV_INFO << "checkVmUserFit - Reservation: " << !pNode->isReserved() << " | TotalCpus: " << pNode->getTotalCpUs() << " vs "<< pNode->getAvailableCpUs() << endl;
                }
            }
            else
                EV_INFO << "checkVmUserFit - WARNING!! empty pointer:" << userVM_Rq->getUserID() << endl;

            nIndex++;
        }
        //Insert the user request in the hashmap
        userRqMap[strUserName]=vmHashSearch;

    }
    else
    {
        EV_INFO << "checkVmUserFit - There isnt enough space: [" << userVM_Rq->getUserID() << nRequestedNodes<< " vs "<< nFreeNodes <<endl;

    }
    if(bRet)
        EV_INFO << "checkVmUserFit - Reserved space for: " << userVM_Rq->getUserID() << endl;
    else
        EV_INFO << "checkVmUserFit - Unable to reserve space for: " << userVM_Rq->getUserID() << endl;

    EV_INFO << "checkVmUserFit- End" << endl;

    return bRet;
}
//Returns the price of the VM given its type
int CloudProviderFirstFitSingleUser::getPriceByVmType(std::string strPrice)
{
    int nRet;
    VirtualMachine* pVmMachine;

    if(strPrice.size()>0)
    {
        for(int i=0;i<vmTypes.size();i++)
        {
            pVmMachine = vmTypes.at(i);
            if(strPrice.compare(pVmMachine->getType()))
            {
                nRet = pVmMachine->getCost();
            }
        }
    }

    return nRet;
}

bool CloudProviderFirstFitSingleUser::checkAppUserFit(SM_UserAPP*& userAPP_Rq)
{
    bool bRet;
    int nArraySize, nPrice;
    bRet = true;
    std::string ip1,ip2,ip3;


    return bRet;
}

void  CloudProviderFirstFitSingleUser::acceptVmRequest(SM_UserVM* userVM_Rq)
{
    EV_INFO << "Accepting request from user:" << userVM_Rq->getUserID() << endl;

    //Fill the message
    //userVM_Rq->setIsAccept(true);
    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);
    userVM_Rq->setResult(SM_VM_Res_Accept);

    //Send the values
    sendResponseMessage(userVM_Rq);

}

void  CloudProviderFirstFitSingleUser::acceptAppRequest(SM_UserAPP* userAPP_Rq)
{
    EV_INFO << "Accepting application submission from the user:" << userAPP_Rq->getUserID() << endl;

    //Fill the message
    userAPP_Rq->setIsResponse(true);
    userAPP_Rq->setOperation(SM_APP_Rsp);
    userAPP_Rq->setResult(SM_APP_Res_Accept);

    //Send the values
    sendResponseMessage(userAPP_Rq);

}
void  CloudProviderFirstFitSingleUser::rejectVmRequest(SM_UserVM* userVM_Rq)
{
    SIMCAN_Message *ms1, *ms2;
    //Create a request_rsp message

    EV_INFO << "Reject VM request from user:" << userVM_Rq->getUserID() << endl;

    userVM_Rq->setIsResponse(true);
    userVM_Rq->setOperation(SM_VM_Req_Rsp);
    userVM_Rq->setResult(SM_VM_Res_Reject);

    //Send the values
    sendResponseMessage(userVM_Rq);
}
void  CloudProviderFirstFitSingleUser::rejectAppRequest(SM_UserAPP* userAPP_Rq)
{
    SIMCAN_Message *ms1, *ms2;
    //Create a request_rsp message

    EV_INFO << "R request from user:" << userAPP_Rq->getUserID() << endl;

    //Fill the message
    userAPP_Rq->setIsResponse(true);
    userAPP_Rq->setOperation(SM_APP_Req);
    userAPP_Rq->setResult(SM_APP_Res_Reject);

    //Send the values
    sendResponseMessage(userAPP_Rq);
}
