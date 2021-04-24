/*
 * IDataCentreCollection.h
 *
 *  Created on: Sep 18, 2018
 *      Author: pablo
 */

#ifndef IDATACENTRECOLLECTION_H_
#define IDATACENTRECOLLECTION_H_

class IDataCentreCollection {

public:
    /**
     * Initialise the collection
     */
    virtual void initialize()=0;
    /**
     * Checks if all the datacentres are full.
     * @return
     */
    virtual bool isCloudFull(int nRequestedCores)=0;

    /**
     * Insert a new node into the datacentre
     * @param nDatacentre
     * @param pNode
     */
    virtual void insertNode(int nDatacentre, NodeResourceInfo* pNode)=0;

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
     * Get the number of available cores of the whole system (all the datacentres).
     * @return
     */
    virtual int getTotalAvailableCores()=0;

    /**
     * Get the total number of cores of the whole system (all the datacentres).
     * @return
     */
    virtual int getTotalCores()=0;

    /**
     * Print a summary about the datacentre sizes.
     */
    virtual void printDCSizes()=0;
};

#endif /* IDATACENTRECOLLECTION_H_ */
