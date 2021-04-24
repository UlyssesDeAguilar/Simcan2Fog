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

#include "DataCentreInfoCollectionReservation.h"

DataCentreInfoCollectionReservation::DataCentreInfoCollectionReservation() {
    dataCentresReserved.clear();
    id2reservedNodeMap.clear();

}

DataCentreInfoCollectionReservation::~DataCentreInfoCollectionReservation() {
    // TODO Auto-generated destructor stub

}

const std::vector<int>& DataCentreInfoCollectionReservation::getReservedNodes() const {
    return reservedNodes;
}

void DataCentreInfoCollectionReservation::setReservedNodes(
        const std::vector<int>& reservedNodes) {
    this->reservedNodes = reservedNodes;
}

int DataCentreInfoCollectionReservation::getTotalReservedAvailableCores()
{
    int nRet;
    DataCentre_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCentresReserved.size();i++)
    {
        pDC = dataCentresReserved.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalAvailableCores();
        }
    }

    return nRet;
}

int DataCentreInfoCollectionReservation::getTotalReservedCores()
{
    int nRet;
    DataCentre_CpuType* pDC;
    nRet = 0;

    for(int i=0;i<dataCentresReserved.size();i++)
    {
        pDC = dataCentresReserved.at(i);
        if(pDC != NULL)
        {
            nRet+=pDC->getTotalCores();
        }
    }

    return nRet;
}

void DataCentreInfoCollectionReservation::insertNode(int nDataCentre, NodeResourceInfo* pNode) {
    int nNodes;
    DataCentre_CpuType* pDataCentreCpuType;
    std::vector<DataCentre_CpuType*>::iterator it;

    if (reservedNodes.at(nDataCentre)>0) {
        pDataCentreCpuType = NULL;
        if(nDataCentre >= dataCentresReserved.size())
        {
            it = dataCentresReserved.begin();
            pDataCentreCpuType = new DataCentre_CpuType(std::to_string(nDataCentre));
            dataCentresReserved.insert(it+nDataCentre,pDataCentreCpuType);
            EV_INFO << "DataCentreInfoCollectionReservation::insertNode - Inserting new reserved datacentre: " << nDataCentre <<endl;
        }
        else
        {
            pDataCentreCpuType = dataCentresReserved.at(nDataCentre);
        }

        if(pNode != NULL && pDataCentreCpuType!= NULL)
        {
            nNodes = pNode->getNumCpUs();
            pDataCentreCpuType->insertNode(pNode,nNodes);
            reservedNodes.at(nDataCentre)--;
        }
        else
        {
            EV_INFO << "DataCentreInfoCollectionReservation::insertNode - WARNING!! Node or datacentre are NULL" <<endl;
        }

    } else {
        DataCentreInfoCollection::insertNode(nDataCentre, pNode);
    }
}

bool DataCentreInfoCollectionReservation::handlePrioritaryVmRequest(NodeResourceRequest*& pVmRequest)
{
    bool bRet;
    int nIndex, nCores;
    std::string strVmId;
    DataCentre_CpuType* pDataCentreCpuType;
    NodeResourceInfo* pInfo;

    //Initialization
    bRet = false;
    nIndex = 0;
    EV_INFO << "DataCentreInfoCollectionReservation::handleVmRequest - Init" <<endl;

    pInfo = NULL;
    //Check if there exist enough space in the datacentres.
    while(nIndex<dataCentresReserved.size() && pInfo==NULL)
    {
        pDataCentreCpuType = dataCentresReserved.at(nIndex);
        if(pDataCentreCpuType != nullptr && pVmRequest != nullptr)
        {
            //Check if there is enough space for this VmRequest
             pInfo = pDataCentreCpuType->handleVmRequest(pVmRequest);
        }
        else
        {
            EV_INFO << "DataCentreInfoCollectionReservation::handleVmRequest - WARNING!!! DataCentre_CpuType NULL" <<endl;
        }

        nIndex++;
    }

    if(pInfo != NULL)
    {
        strVmId = pVmRequest->getVmId();
        EV_INFO << "DataCentreInfoCollectionReservation::handleVmRequest - Storing in the hashmap: "<< strVmId << " | user: " << pVmRequest->getUserName() << endl;
        //Insert the node into a hashmap: IP->nodePointer
        id2reservedNodeMap[strVmId]=pInfo;

        //Fill the resources
        pVmRequest->setIp(pInfo->getIp());
        pVmRequest->setStartTime(simTime().dbl());
        bRet = true;
    }
    else
    {
        EV_INFO << "DataCentreInfoCollectionReservation::handleVmRequest - WARNING! There isnt space for the VM" <<endl;
    }

    EV_INFO << "DataCentreInfoCollectionReservation::handleVmRequest - End" <<endl;
    return bRet;
}

/**
 * This method is used for releasing a VM request
 * @param strVmId VM identifier
 * @return The result of the process
 */
bool DataCentreInfoCollectionReservation::freeVmRequest(std::string strVmId)
{
    bool bRet;
    NodeResourceInfo* pInfo;
    DataCentre_CpuType* pDataCentreCpuType;
    int nDataCentre;

    EV_TRACE << "DataCentreInfoCollection::freeVmRequest - Init" <<endl;

    bRet=false;
    if(id2reservedNodeMap.find(strVmId) != id2reservedNodeMap.end())
    {
        EV_TRACE << "DataCentreInfoCollection::freeVmRequest - The VM with ID " << strVmId << " has been found in id2nodeMap" <<endl;
        pInfo = id2reservedNodeMap.at(strVmId);

        nDataCentre = pInfo->getDataCentre();
        if(nDataCentre != -1)
        {
            pInfo->printAvailableResources();
            pDataCentreCpuType = dataCentresReserved.at(nDataCentre);
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

    if (!bRet){
        bRet = DataCentreInfoCollection::freeVmRequest(strVmId);
    }
    return bRet;
}

/**
 * This method has been coded for debugging, due to some racks seems to be empty
 */
void DataCentreInfoCollectionReservation::printDCSizes()
{
    DataCentre_CpuType* pDC;

    EV_TRACE << "DataCentreInfoCollectionReservation::printDCSizes - Printing the reserved datacentre collection" <<endl;

    for(int i=0;i<dataCentresReserved.size();i++)
    {
        pDC = dataCentresReserved.at(i);
        if(pDC != NULL)
        {
            EV_TRACE << i << " "<< pDC->getAvailableResources();
        }
    }

    EV_TRACE << "DataCentreInfoCollectionReservation::printDCSizes - End" <<endl;

    DataCentreInfoCollection::printDCSizes();
}

bool DataCentreInfoCollectionReservation::isReservedCloudFull(int nRequestedCores)
{
    bool bRet;

    bRet = nRequestedCores < getTotalReservedAvailableCores();

    return bRet;
}
