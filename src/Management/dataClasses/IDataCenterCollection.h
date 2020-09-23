/*
 * IDataCenterCollection.h
 *
 *  Created on: Sep 18, 2018
 *      Author: pablo
 */

#ifndef IDATACENTERCOLLECTION_H_
#define IDATACENTERCOLLECTION_H_

class IDataCenterCollection {

public:
    /**
     * Initialise the collection
     */
    virtual void initialize()=0;
    /**
     * Checks if all the datacenters are full.
     * @return
     */
    virtual bool isCloudFull(int nRequestedCores)=0;

    /**
     * Insert a new node into the datacenter
     * @param nDatacenter
     * @param pNode
     */
    virtual void insertNode(int nDatacenter, NodeResourceInfo* pNode)=0;

    /**
     * This method handles a VM request
     * @param pVmRequest VM request
     * @return TRUE if this VM is accepted. FALSE i.o.c.
     */
    virtual bool handleVmRequest(NodeResourceRequest*& pVmRequest)=0;

    /**
     * Free a VM request
     * @param strVmId Identifier of the VM
     * @return TRUE if this VM has been released successfully. FALSE i.o.c.
     */
    virtual bool freeVmRequest(std::string strVmId)=0;

    /**
     * Get the number of available cores of the whole system (all the datacenters).
     * @return
     */
    virtual int getTotalAvailableCores()=0;

    /**
     * Get the total number of cores of the whole system (all the datacenters).
     * @return
     */
    virtual int getTotalCores()=0;

    /**
     * Print a summary about the datacenter sizes.
     */
    virtual void printDCSizes()=0;
};

#endif /* IDATACENTERCOLLECTION_H_ */
