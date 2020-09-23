/*
 * DataCenterInfoCollection.h
 *
 *  Created on: Jun 27, 2017
 *      Author: pablo
 */

#ifndef DATACENTERINFOCOLLECTION_H_
#define DATACENTERINFOCOLLECTION_H_

#include "DataCenter_CpuType.h"
#include "IDataCenterCollection.h"
/**
 * This class is used to handle and manage the VMs that are requested to this datacenter. For this, this class contains a simplified version, that contains the information necessary,
 * of the datacenter, nodes and its resources.
 */
class DataCenterInfoCollection: public IDataCenterCollection {

private:

    /** Map used to store the (accepted) requests performed by each user*/
    std::map<std::string, NodeResourceInfo*> id2nodeMap;

    /** Vector that contains a collection of DataCenters that structures Nodes by CPU type */
    std::vector<DataCenter_CpuType*> dataCenters;

public:

    /**
     * Constructor.
     */
    DataCenterInfoCollection();

    /**
     * Destructor.
     */
    virtual ~DataCenterInfoCollection();


    /**
     * Initialise the datacentre
     */
    virtual void initialize();

    /**
     * Checks if all the datacenters are full.
     * @return
     */
    virtual bool isCloudFull(int nRequestedCores);

    /**
     * Insert a new node into the datacenter
     * @param nDatacenter
     * @param pNode
     */
    virtual void insertNode(int nDataCenter, NodeResourceInfo* pNode);

    /**
     * This method handles a VM request
     * @param pVmRequest VM request
     * @return TRUE if this VM is accepted. FALSE i.o.c.
     */
    virtual bool handleVmRequest(NodeResourceRequest*& pVmRequest);

    /**
     * Free a VM request
     * @param strVmId Identifier of the VM
     * @return TRUE if this VM has been released successfully. FALSE i.o.c.
     */
    virtual bool freeVmRequest(std::string strVmId);

    /**
     * Get the number of available cores of the whole system (all the datacenters).
     * @return
     */
    virtual int getTotalAvailableCores();

    /**
     * Get the total number of cores of the whole system (all the datacenters).
     * @return
     */
    virtual int getTotalCores();

    /**
     * Print a summary about the datacenter sizes.
     */
    virtual void printDCSizes();
};

#endif /* DATACENTERINFOCOLLECTION_H_ */
