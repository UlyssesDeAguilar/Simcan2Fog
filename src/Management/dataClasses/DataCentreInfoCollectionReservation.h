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

#ifndef MANAGEMENT_DATACLASSES_DATACENTREINFOCOLLECTIONRESERVATION_H_
#define MANAGEMENT_DATACLASSES_DATACENTREINFOCOLLECTIONRESERVATION_H_

#include "DataCentreInfoCollection.h"

class DataCentreInfoCollectionReservation: public DataCentreInfoCollection {
public:
    DataCentreInfoCollectionReservation();
    virtual ~DataCentreInfoCollectionReservation();


    /**
     * Checks if all the datacentres are full.
     * @return
     */
    virtual bool isReservedCloudFull(int nRequestedCores);

    /**
     * Insert a new node into the datacentre
     * @param nDatacentre
     * @param pNode
     */
    virtual void insertNode(int nDataCentre, NodeResourceInfo* pNode) override;

    /**
     * This method handles a VM request
     * @param pVmRequest VM request
     * @return TRUE if this VM is accepted. FALSE i.o.c.
     */
    virtual bool handlePrioritaryVmRequest(NodeResourceRequest*& pVmRequest);

    /**
     * Free a VM request
     * @param strVmId Identifier of the VM
     * @return TRUE if this VM has been released successfully. FALSE i.o.c.
     */
    virtual bool freeVmRequest(std::string strVmId) override;

    /**
     * Get the number of available cores of the whole system (all the datacentres).
     * @return
     */
    virtual int getTotalReservedAvailableCores();

    /**
     * Get the total number of cores of the whole system (all the datacentres).
     * @return
     */
    virtual int getTotalReservedCores();

    /**
     * Print a summary about the datacentre sizes.
     */
    virtual void printDCSizes() override;

    const std::vector<int>& getReservedNodes() const;
    void setReservedNodes(const std::vector<int>& reservedNodes);

protected:
    std::vector<int> reservedNodes;
    /** Vector that contains a collection of DataCentres that structures Nodes by CPU type */
    std::vector<DataCentre_CpuType*> dataCentresReserved;
    /** Map used to store the (accepted) requests performed by each user*/
    std::map<std::string, NodeResourceInfo*> id2reservedNodeMap;
};

#endif /* MANAGEMENT_DATACLASSES_DATACENTREINFOCOLLECTIONRESERVATION_H_ */
