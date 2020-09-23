//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "DataCenterInfoCollectionReservation.h"

DataCenterInfoCollectionReservation::DataCenterInfoCollectionReservation() {
    dataCentersReserved.clear();
    id2reservedNodeMap.clear();

}

DataCenterInfoCollectionReservation::~DataCenterInfoCollectionReservation() {
    // TODO Auto-generated destructor stub

}

const std::vector<int>& DataCenterInfoCollectionReservation::getReservedNodes() const {
    return reservedNodes;
}

void DataCenterInfoCollectionReservation::setReservedNodes(
        const std::vector<int>& reservedNodes) {
    this->reservedNodes = reservedNodes;
}

int DataCenterInfoCollectionReservation::getTotalReservedAvailableCores()
{
    int nRet;
    DataCenter_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCentersReserved.size();i++)
    {
        pDC = dataCentersReserved.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalAvailableCores();
        }
    }

    return nRet;
}

int DataCenterInfoCollectionReservation::getTotalReservedCores()
{
    int nRet;
    DataCenter_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCentersReserved.size();i++)
    {
        pDC = dataCentersReserved.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalCores();
        }
    }

    return nRet;
}

void DataCenterInfoCollectionReservation::insertNode(int nDataCenter, NodeResourceInfo* pNode) {
    int nNodes;
    DataCenter_CpuType* pDataCenterCpuType;
    std::vector<DataCenter_CpuType*>::iterator it;

    if (reservedNodes.at(nDataCenter)>0) {
        pDataCenterCpuType = NULL;
        if(nDataCenter >= dataCentersReserved.size())
        {
            it = dataCentersReserved.begin();
            pDataCenterCpuType = new DataCenter_CpuType(std::to_string(nDataCenter));
            dataCentersReserved.insert(it+nDataCenter,pDataCenterCpuType);
            EV_INFO << "DataCenterInfoCollectionReservation::insertNode - Inserting new reserved datacenter: " << nDataCenter <<endl;
        }
        else
        {
            pDataCenterCpuType = dataCentersReserved.at(nDataCenter);
        }

        if(pNode != NULL && pDataCenterCpuType!= NULL)
        {
            nNodes = pNode->getNumCpUs();
            pDataCenterCpuType->insertNode(pNode,nNodes);
            reservedNodes.at(nDataCenter)--;
        }
        else
        {
            EV_INFO << "DataCenterInfoCollectionReservation::insertNode - WARNING!! Node or datacenter are NULL" <<endl;
        }

    } else {
        DataCenterInfoCollection::insertNode(nDataCenter, pNode);
    }
}

bool DataCenterInfoCollectionReservation::handlePrioritaryVmRequest(NodeResourceRequest*& pVmRequest)
{
    bool bRet;
    int nIndex, nCores;
    std::string strVmId;
    DataCenter_CpuType* pDataCenterCpuType;
    NodeResourceInfo* pInfo;

    //Initialization
    bRet = false;
    nIndex = 0;
    EV_INFO << "DataCenterInfoCollectionReservation::handleVmRequest - Init" <<endl;

    pInfo = NULL;
    //Check if there exist enough space in the datacenters.
    while(nIndex<dataCentersReserved.size() && pInfo==NULL)
    {
        pDataCenterCpuType = dataCentersReserved.at(nIndex);
        if(pDataCenterCpuType != nullptr && pVmRequest != nullptr)
        {
            //Check if there is enough space for this VmRequest
             pInfo = pDataCenterCpuType->handleVmRequest(pVmRequest);
        }
        else
        {
            EV_INFO << "DataCenterInfoCollectionReservation::handleVmRequest - WARNING!!! DataCenter_CpuType NULL" <<endl;
        }

        nIndex++;
    }

    if(pInfo != NULL)
    {
        strVmId = pVmRequest->getVmId();
        EV_INFO << "DataCenterInfoCollectionReservation::handleVmRequest - Storing in the hashmap: "<< strVmId << " | user: " << pVmRequest->getUserName() << endl;
        //Insert the node into a hashmap: IP->nodePointer
        id2reservedNodeMap[strVmId]=pInfo;

        //Fill the resources
        pVmRequest->setIp(pInfo->getIp());
        pVmRequest->setStartTime(simTime().dbl());
        bRet = true;
    }
    else
    {
        EV_INFO << "DataCenterInfoCollectionReservation::handleVmRequest - WARNING! There isnt space for the VM" <<endl;
    }

    EV_INFO << "DataCenterInfoCollectionReservation::handleVmRequest - End" <<endl;
    return bRet;
}

/**
 * This method is used for releasing a VM request
 * @param strVmId VM identifier
 * @return The result of the process
 */
bool DataCenterInfoCollectionReservation::freeVmRequest(std::string strVmId)
{
    bool bRet;
    NodeResourceInfo* pInfo;
    DataCenter_CpuType* pDataCenterCpuType;
    int nDataCenter;

    EV_TRACE << "DataCenterInfoCollection::freeVmRequest - Init" <<endl;

    bRet=false;
    if(id2reservedNodeMap.find(strVmId) != id2reservedNodeMap.end())
    {
        EV_TRACE << "DataCenterInfoCollection::freeVmRequest - The VM with ID " << strVmId << " has been found in id2nodeMap" <<endl;
        pInfo = id2reservedNodeMap.at(strVmId);

        nDataCenter = pInfo->getDataCenter();
        if(nDataCenter != -1)
        {
            pInfo->printAvailableResources();
            pDataCenterCpuType = dataCentersReserved.at(nDataCenter);
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

    if (!bRet){
        bRet = DataCenterInfoCollection::freeVmRequest(strVmId);
    }
    return bRet;
}

/**
 * This method has been coded for debugging, due to some racks seems to be empty
 */
void DataCenterInfoCollectionReservation::printDCSizes()
{
    DataCenter_CpuType* pDC;

    EV_TRACE << "DataCenterInfoCollectionReservation::printDCSizes - Printing the reserved datacenter collection" <<endl;

    for(int i=0;i<dataCentersReserved.size();i++)
    {
        pDC = dataCentersReserved.at(i);
        if(pDC != NULL)
        {
            EV_TRACE << i << " "<< pDC->getAvailableResources();
        }
    }

    EV_TRACE << "DataCenterInfoCollectionReservation::printDCSizes - End" <<endl;

    DataCenterInfoCollection::printDCSizes();
}

bool DataCenterInfoCollectionReservation::isReservedCloudFull(int nRequestedCores)
{
    bool bRet;

    bRet = nRequestedCores < getTotalReservedAvailableCores();

    return bRet;
}
