/*
 * DataCenterInfoCollection.cpp
 *
 *  Created on: Jun 27, 2017
 *      Author: pablo
 */

#include "DataCenterInfoCollection.h"

DataCenterInfoCollection::DataCenterInfoCollection() {

    dataCenters.clear();
    id2nodeMap.clear();
}
DataCenterInfoCollection::~DataCenterInfoCollection() {
}
bool DataCenterInfoCollection::isCloudFull(int nRequestedCores)
{
    bool bRet;

    bRet = nRequestedCores < getTotalAvailableCores();

    return bRet;
}
void DataCenterInfoCollection::initialize()
{
}
int DataCenterInfoCollection::getTotalAvailableCores()
{
    int nRet;
    DataCenter_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCenters.size();i++)
    {
        pDC = dataCenters.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalAvailableCores();
        }
    }

    return nRet;
}
int DataCenterInfoCollection::getTotalCores()
{
    int nRet;
    DataCenter_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCenters.size();i++)
    {
        pDC = dataCenters.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalCores();
        }
    }

    return nRet;
}
void DataCenterInfoCollection::insertNode(int nDataCenter, NodeResourceInfo* pNode)
{
    int nNodes;
    DataCenter_CpuType* pDataCenterCpuType;
    std::vector<DataCenter_CpuType*>::iterator it;

    pDataCenterCpuType = NULL;

    if(nDataCenter >= dataCenters.size())
    {
        it = dataCenters.begin();
        pDataCenterCpuType = new DataCenter_CpuType(std::to_string(nDataCenter));
        dataCenters.insert(it+nDataCenter,pDataCenterCpuType);
        EV_INFO << "DataCenterInfoCollection::insertNode - Inserting new datacenter: " << nDataCenter <<endl;
    }
    else
    {
        pDataCenterCpuType = dataCenters.at(nDataCenter);
    }

    if(pNode != NULL && pDataCenterCpuType!= NULL)
    {
        nNodes = pNode->getNumCpUs();
        pDataCenterCpuType->insertNode(pNode,nNodes);
    }
    else
    {
        EV_INFO << "DataCenterInfoCollection::insertNode - WARNING!! Node or datacenter are NULL" <<endl;
    }
}
bool DataCenterInfoCollection::handleVmRequest(NodeResourceRequest*& pVmRequest)
{
    bool bRet;
    int nIndex, nCores;
    std::string strVmId;
    DataCenter_CpuType* pDataCenterCpuType;
    NodeResourceInfo* pInfo;

    //Initialization
    bRet = false;
    nIndex = 0;
    EV_INFO << "DataCenterInfoCollection::handleVmRequest - Init" <<endl;

    pInfo = NULL;
    //Check if there exist enough space in the datacenters.
    while(nIndex<dataCenters.size() && pInfo==NULL)
    {
        pDataCenterCpuType = dataCenters.at(nIndex);
        if(pDataCenterCpuType != nullptr && pVmRequest != nullptr)
        {
            //Check if there is enough space for this VmRequest
             pInfo = pDataCenterCpuType->handleVmRequest(pVmRequest);
        }
        else
        {
            EV_INFO << "DataCenterInfoCollection::handleVmRequest - WARNING!!! DataCenter_CpuType NULL" <<endl;
        }

        nIndex++;
    }

    if(pInfo != NULL)
    {
        strVmId = pVmRequest->getVmId();
        EV_INFO << "DataCenterInfoCollection::handleVmRequest - Storing in the hashmap: "<< strVmId << " | user: " << pVmRequest->getUserName() << endl;
        //Insert the node into a hashmap: IP->nodePointer
        id2nodeMap[strVmId]=pInfo;

        //Fill the resources
        pVmRequest->setIp(pInfo->getIp());
        pVmRequest->setStartTime(simTime().dbl());
        bRet = true;
    }
    else
    {
        EV_INFO << "DataCenterInfoCollection::handleVmRequest - WARNING! There isnt space for the VM" <<endl;
    }

    EV_INFO << "DataCenterInfoCollection::handleVmRequest - End" <<endl;
    return bRet;
}
/**
 * This method is used for releasing a VM request
 * @param strVmId VM identifier
 * @return The result of the process
 */
bool DataCenterInfoCollection::freeVmRequest(std::string strVmId)
{
    bool bRet;
    NodeResourceInfo* pInfo;
    DataCenter_CpuType* pDataCenterCpuType;
    int nDataCenter;

    EV_TRACE << "DataCenterInfoCollection::freeVmRequest - Init" <<endl;

    bRet=false;
    if(id2nodeMap.find(strVmId) != id2nodeMap.end())
    {
        EV_TRACE << "DataCenterInfoCollection::freeVmRequest - The VM with ID " << strVmId << " has been found in id2nodeMap" <<endl;
        pInfo = id2nodeMap.at(strVmId);

        nDataCenter = pInfo->getDataCenter();
        if(nDataCenter != -1)
        {
            pInfo->printAvailableResources();
            pDataCenterCpuType = dataCenters.at(nDataCenter);
            pDataCenterCpuType->freeVmFromNode(strVmId,pInfo);
            bRet = true;
        }
        else
        {
            EV_INFO << "DataCenterInfoCollection::freeVmRequest - The dataCenter Id is invalid "<< nDataCenter <<endl;
        }
    }
    else
    {
        EV_INFO << "DataCenterInfoCollection::freeVmRequest - The VM with ID " << strVmId << " NOT found in id2nodeMap" <<endl;
    }

    EV_TRACE << "DataCenterInfoCollection::freeVmRequest - End" <<endl;


    return bRet;
}
/**
 * This method has been coded for debugging, due to some racks seems to be empty
 */
void DataCenterInfoCollection::printDCSizes()
{
    DataCenter_CpuType* pDC;

    EV_TRACE << "DataCenterInfoCollection::printDCSizes - Printing the datacenter collection" <<endl;

    for(int i=0;i<dataCenters.size();i++)
    {
        pDC = dataCenters.at(i);
        if(pDC != NULL)
        {
            EV_TRACE << i << " "<< pDC->getAvailableResources();
        }
    }

    EV_TRACE << "DataCenterInfoCollection::printDCSizes - End" <<endl;
}
