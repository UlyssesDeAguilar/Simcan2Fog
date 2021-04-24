/*
 * DataCentreInfoCollection.cpp
 *
 *  Created on: Jun 27, 2017
 *      Author: pablo
 */

#include "DataCentreInfoCollection.h"

DataCentreInfoCollection::DataCentreInfoCollection() {

    dataCentres.clear();
    id2nodeMap.clear();
}
DataCentreInfoCollection::~DataCentreInfoCollection() {
}
bool DataCentreInfoCollection::isCloudFull(int nRequestedCores)
{
    bool bRet;

    bRet = nRequestedCores < getTotalAvailableCores();

    return bRet;
}
void DataCentreInfoCollection::initialize()
{
}
int DataCentreInfoCollection::getTotalAvailableCores()
{
    int nRet;
    DataCentre_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCentres.size();i++)
    {
        pDC = dataCentres.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalAvailableCores();
        }
    }

    return nRet;
}
int DataCentreInfoCollection::getTotalCores()
{
    int nRet;
    DataCentre_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCentres.size();i++)
    {
        pDC = dataCentres.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalCores();
        }
    }

    return nRet;
}
void DataCentreInfoCollection::insertNode(int nDataCentre, NodeResourceInfo* pNode)
{
    int nNodes;
    DataCentre_CpuType* pDataCentreCpuType;
    std::vector<DataCentre_CpuType*>::iterator it;

    pDataCentreCpuType = NULL;

    if(nDataCentre >= dataCentres.size())
    {
        it = dataCentres.begin();
        pDataCentreCpuType = new DataCentre_CpuType(std::to_string(nDataCentre));
        dataCentres.insert(it+nDataCentre,pDataCentreCpuType);
        EV_INFO << "DataCentreInfoCollection::insertNode - Inserting new datacentre: " << nDataCentre <<endl;
    }
    else
    {
        pDataCentreCpuType = dataCentres.at(nDataCentre);
    }

    if(pNode != NULL && pDataCentreCpuType!= NULL)
    {
        nNodes = pNode->getNumCpUs();
        pDataCentreCpuType->insertNode(pNode,nNodes);
    }
    else
    {
        EV_INFO << "DataCentreInfoCollection::insertNode - WARNING!! Node or datacentre are NULL" <<endl;
    }
}
bool DataCentreInfoCollection::handleVmRequest(NodeResourceRequest*& pVmRequest)
{
    bool bRet;
    int nIndex, nCores;
    std::string strVmId;
    DataCentre_CpuType* pDataCentreCpuType;
    NodeResourceInfo* pInfo;

    //Initialization
    bRet = false;
    nIndex = 0;
    EV_INFO << "DataCentreInfoCollection::handleVmRequest - Init" <<endl;

    pInfo = NULL;
    //Check if there exist enough space in the datacentres.
    while(nIndex<dataCentres.size() && pInfo==NULL)
    {
        pDataCentreCpuType = dataCentres.at(nIndex);
        if(pDataCentreCpuType != nullptr && pVmRequest != nullptr)
        {
            //Check if there is enough space for this VmRequest
             pInfo = pDataCentreCpuType->handleVmRequest(pVmRequest);
        }
        else
        {
            EV_INFO << "DataCentreInfoCollection::handleVmRequest - WARNING!!! DataCentre_CpuType NULL" <<endl;
        }

        nIndex++;
    }

    if(pInfo != NULL)
    {
        strVmId = pVmRequest->getVmId();
        EV_INFO << "DataCentreInfoCollection::handleVmRequest - Storing in the hashmap: "<< strVmId << " | user: " << pVmRequest->getUserName() << endl;
        //Insert the node into a hashmap: IP->nodePointer
        id2nodeMap[strVmId]=pInfo;

        //Fill the resources
        pVmRequest->setIp(pInfo->getIp());
        pVmRequest->setStartTime(simTime().dbl());
        bRet = true;
    }
    else
    {
        EV_INFO << "DataCentreInfoCollection::handleVmRequest - WARNING! There isnt space for the VM" <<endl;
    }

    EV_INFO << "DataCentreInfoCollection::handleVmRequest - End" <<endl;
    return bRet;
}
/**
 * This method is used for releasing a VM request
 * @param strVmId VM identifier
 * @return The result of the process
 */
bool DataCentreInfoCollection::freeVmRequest(std::string strVmId)
{
    bool bRet;
    NodeResourceInfo* pInfo;
    DataCentre_CpuType* pDataCentreCpuType;
    int nDataCentre;

    EV_TRACE << "DataCentreInfoCollection::freeVmRequest - Init" <<endl;

    bRet=false;
    if(id2nodeMap.find(strVmId) != id2nodeMap.end())
    {
        EV_TRACE << "DataCentreInfoCollection::freeVmRequest - The VM with ID " << strVmId << " has been found in id2nodeMap" <<endl;
        pInfo = id2nodeMap.at(strVmId);

        nDataCentre = pInfo->getDataCentre();
        if(nDataCentre != -1)
        {
            pInfo->printAvailableResources();
            pDataCentreCpuType = dataCentres.at(nDataCentre);
            pDataCentreCpuType->freeVmFromNode(strVmId,pInfo);
            bRet = true;
        }
        else
        {
            EV_INFO << "DataCentreInfoCollection::freeVmRequest - The dataCentre Id is invalid "<< nDataCentre <<endl;
        }
    }
    else
    {
        EV_INFO << "DataCentreInfoCollection::freeVmRequest - The VM with ID " << strVmId << " NOT found in id2nodeMap" <<endl;
    }

    EV_TRACE << "DataCentreInfoCollection::freeVmRequest - End" <<endl;


    return bRet;
}
/**
 * This method has been coded for debugging, due to some racks seems to be empty
 */
void DataCentreInfoCollection::printDCSizes()
{
    DataCentre_CpuType* pDC;

    EV_TRACE << "DataCentreInfoCollection::printDCSizes - Printing the datacentre collection" <<endl;

    for(int i=0;i<dataCentres.size();i++)
    {
        pDC = dataCentres.at(i);
        if(pDC != NULL)
        {
            EV_TRACE << i << " "<< pDC->getAvailableResources();
        }
    }

    EV_TRACE << "DataCentreInfoCollection::printDCSizes - End" <<endl;
}
