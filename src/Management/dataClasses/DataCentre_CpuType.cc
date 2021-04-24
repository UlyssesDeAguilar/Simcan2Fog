#include "DataCentre_CpuType.h"

DataCentre_CpuType::DataCentre_CpuType(std::string name){
    this->name = name;
    nTotalAvailableCores = 0;
    nTotalCores=0;
}
DataCentre_CpuType::~DataCentre_CpuType(){
}
const std::string& DataCentre_CpuType::getName() const {
    return name;
}
void DataCentre_CpuType::insertNode (NodeResourceInfo* newNode, int numCpus){

    //Update the total number of cores of the data-centre
    nTotalAvailableCores+=numCpus;
    nTotalCores+=numCpus;

    EV_TRACE << "DataCentre_CpuType::insertNode - Inserting new node, size: " << numCpus << endl;
    switch (numCpus){

        // Single-core
        case 1:
            nodes_1core.push_back(newNode);
            free_nodes_1core.push_back(newNode);
            break;

        // Dual-core
        case 2:
            nodes_2cores.push_back(newNode);
            free_nodes_2cores.push_back(newNode);
            break;

        // Quad-core
        case 4:
            nodes_4cores.push_back(newNode);
            free_nodes_4cores.push_back(newNode);
            break;

        // Octa-core
        case 8:
            nodes_8cores.push_back(newNode);
            free_nodes_8cores.push_back(newNode);
            break;

        // 16-core
        case 16:
            nodes_16cores.push_back(newNode);
            free_nodes_16cores.push_back(newNode);
            break;

        // 32-core
        case 32:
            nodes_32cores.push_back(newNode);
            free_nodes_32cores.push_back(newNode);
            break;

        default:
            throw omnetpp::cRuntimeError("[DataCentre_CpuType] Wrong list. There is no Node with %d CPU cores!", numCpus);
    }
}
std::vector <NodeResourceInfo*>* DataCentre_CpuType::getNodeListByCores(int numCores)
{
    std::vector <NodeResourceInfo*>* pRetList;
    switch (numCores){

         // Single-core
         case 1:
             pRetList = &nodes_1core;
             break;

         // Dual-core
         case 2:
             pRetList = &nodes_2cores;
             break;

         // Quad-core
         case 4:
             pRetList = &nodes_4cores;
             break;

         // Octa-core
         case 8:
             pRetList = &nodes_8cores;
             break;

         // 16-core
         case 16:
             pRetList = &nodes_16cores;
             break;

         // 32-core
         case 32:
             pRetList = &nodes_32cores;
             break;

         default:
             throw omnetpp::cRuntimeError("[DataCentre_CpuType] Wrong list. There is no Node with %d CPU cores!", numCores);
     }
    return pRetList;
}
std::vector <NodeResourceInfo*>* DataCentre_CpuType::getFreeNodeListByCores(int numCores)
{
    std::vector <NodeResourceInfo*>* pRetList;

    EV_TRACE << "DataCentre_CpuType::getFreeNodeListByCores - Init" << endl;
    switch (numCores){

         // Single-core
         case 1:
             EV_DEBUG << "DataCentre_CpuType::getFreeNodeListByCores - using 1 core list" << endl;
             pRetList = &free_nodes_1core;
             break;

         // Dual-core
         case 2:
             EV_DEBUG << "DataCentre_CpuType::getFreeNodeListByCores - using 2 core list" << endl;
             pRetList = &free_nodes_2cores;
             break;

         // Quad-core
         case 4:
             EV_DEBUG << "DataCentre_CpuType::getFreeNodeListByCores - using 4 core list" << endl;
             pRetList = &free_nodes_4cores;
             break;

         // Octa-core
         case 8:
             EV_DEBUG << "DataCentre_CpuType::getFreeNodeListByCores - using 8 core list" << endl;
             pRetList = &free_nodes_8cores;
             break;

         // 16-core
         case 16:
             EV_DEBUG << "DataCentre_CpuType::getFreeNodeListByCores - using 16 core list" << endl;
             pRetList = &free_nodes_16cores;
             break;

         // 32-core
         case 32:
             EV_DEBUG << "DataCentre_CpuType::getFreeNodeListByCores - using 32 core list" << endl;
             pRetList = &free_nodes_32cores;
             break;

         default:
             throw omnetpp::cRuntimeError("[DataCentre_CpuType] Wrong list. There is no Node with %d CPU cores!", numCores);
     }
    EV_TRACE << "DataCentre_CpuType::getFreeNodeListByCores - End" << endl;
    return pRetList;
}
NodeResourceInfo* DataCentre_CpuType::allocNewResources(NodeResourceRequest* pVmRequest, std::vector <NodeResourceInfo*>*& pList)
{
    bool bAlloc;
    int nIndex;
    NodeResourceInfo* pNode;

    nIndex = 0;
    bAlloc = false;

    EV_DEBUG << "DataCentre_CpuType::allocNewResources - Init | RqCores: " << pVmRequest->getTotalCpus() << " | NumNodes: " << pList->size() << " | TotalDCcores: " << nTotalAvailableCores << endl;

    if(pList == nullptr)
        EV_DEBUG << "DataCentre_CpuType::allocNewResources - Bad choice :(" << endl;

    while(!bAlloc && nIndex<pList->size())
    {
        pNode = pList->at(nIndex);
        if(pNode != NULL)
        {
            if(pNode->hasFit(pVmRequest))
            {
                //Insert the user request
                pNode->insertUserRequest(pVmRequest);

                //Update values
                nTotalAvailableCores-=pVmRequest->getTotalCpus();
                bAlloc=true;
            }
            else
                EV_TRACE << "DataCentre_CpuType::allocNewResources - " << nIndex << " FULL!!!!" << endl;
        }
        else
            EV_INFO << "DataCentre_CpuType::allocNewResources - WARNING empty node!!!!" << endl;

        nIndex++;
    }
    if(pList->size()==0)
        EV_INFO << "DataCentre_CpuType::allocNodeInList - WARNING empty list!!!!" << endl;

    if (!bAlloc)
        pNode = NULL;

    EV_DEBUG << "DataCentre_CpuType::allocNewResources - End" << endl;

    return pNode;
}
NodeResourceInfo* DataCentre_CpuType::handleVmRequest(NodeResourceRequest* pVmRequest)
{
    NodeResourceInfo* pNode;
    int nRequestedCores, nNodeCores;
    std::vector <NodeResourceInfo*>* pArrayNodeList;

    nRequestedCores = pVmRequest->getTotalCpus();
    nNodeCores = nRequestedCores;
    pNode=NULL;

    EV_TRACE << "DataCentre_CpuType::handleVmRequest - Init" << endl;
    EV_DEBUG << "DataCentre_CpuType::handleVmRequest - Vm: " << pVmRequest->getVmId()<< ", cores: " << nRequestedCores << endl;
    while(nNodeCores<=32 && pNode==NULL)
    {
        pArrayNodeList = getFreeNodeListByCores(nNodeCores);
        if(pArrayNodeList != NULL && pArrayNodeList->size()>0)
        {
            pNode = allocNewResources(pVmRequest, pArrayNodeList);
            if(pNode == NULL)
            {
                EV_DEBUG << "DataCentre_CpuType::handleVmRequest - Full list: "<< nNodeCores << endl;
            }
        }
        else
        {
            //The list has not enough space
            EV_DEBUG << "DataCentre_CpuType::handleVmRequest - Full list: "<< nNodeCores << endl;
        }
        //This value is duplicated due to, it is neccesary to increase the total of cores necessaries
        //to handle the VM.
        nNodeCores = nNodeCores*2;
    }
    EV_TRACE << "DataCentre_CpuType::handleVmRequest - End" << endl;
    return pNode;
}
void DataCentre_CpuType::freeVmFromNode(std::string strVmId, NodeResourceInfo* pResInfo)
{
    int nTotalCpus, nResourceIndex, nIndex, nCoreList;
    std::vector <NodeResourceInfo*>* pNodeList;

    EV_TRACE << "DataCentre_CpuType::freeVmFromNode - Init, free Vm: " << strVmId << endl;

    if(pResInfo != NULL)
    {
        //Get the total CPUs reserved for this user!
        nTotalCpus = pResInfo->getTotalCpusById(strVmId);
        //Get the total cores of the Node resource in order to find it in the List
        nCoreList = pResInfo->getNumCpUs();

        pNodeList = getFreeNodeListByCores(nCoreList);

        //In this case, we must to replace the node
        nIndex = findResourceIndex(pNodeList, pResInfo->getIp());
        if(nIndex != -1)
        {
            pResInfo->freeVmById(strVmId);

            //Update the cpu counter:
            nTotalAvailableCores+= nTotalCpus;
            EV_DEBUG << "DataCentre_CpuType::freeVmFromNode - The VM has been re-inserted to the system, total cores: "<< nTotalCpus << " | Available cores: " << nTotalAvailableCores << endl;
        }
        else
            EV_INFO << "DataCentre_CpuType::freeVmFromNode - The resource has not been found: " << strVmId << "| Total cores:"<< nTotalCpus << " | Available cores: " << nTotalAvailableCores << endl;
    }
}
int DataCentre_CpuType::findResourceIndex(std::vector <NodeResourceInfo*>*& pList, std::string strResourceId)
{
    int nRet, nIndex;
    bool bFound;
    NodeResourceInfo* pResNode;
    EV_TRACE << "DataCentre_CpuType::findResourceIndex - Init" << endl;

    bFound = false;
    nRet = -1;
    nIndex=0;
    EV_INFO << "DataCentre_CpuType::findResourceIndex - Searching the node with ip: " << strResourceId << endl;

    if(pList != nullptr || pList->size() ==0)
    {
        while(!bFound && nIndex<pList->size())
        {
            pResNode = pList->at(nIndex);
            if(strResourceId.find(pResNode->getIp()) == 0)
            {
                nRet = nIndex;
                bFound = true;
            }
            else
                nIndex++;
        }
    }
    else
        EV_INFO << "DataCentre_CpuType::findResourceIndex - The provided list is empty or NULL!!" << endl;

    if(bFound)
    {
        EV_DEBUG << "DataCentre_CpuType::findResourceIndex - The element has been found at position: " << nRet << endl;

    }
    EV_TRACE << "DataCentre_CpuType::findResourceIndex - End" << endl;

    return nRet;
}
std::string DataCentre_CpuType::getAvailableResources()
{
    std::ostringstream info;

    info << "datacentre["<<name<<"] Available cores: " << nTotalAvailableCores << " [";
    info << "1:" << free_nodes_1core.size() << ", ";
    info << "2:" << free_nodes_2cores.size() << ", ";
    info << "4:" << free_nodes_4cores.size() << ", ";
    info << "8:" << free_nodes_8cores.size() << ", ";
    info << "16:" << free_nodes_16cores.size() << ", ";
    info << "32:" << free_nodes_32cores.size() << "]" << endl;

    return info.str();
}
