#ifndef DATACENTRECPUTYPE_H_
#define DATACENTRECPUTYPE_H_

#include <string.h>
#include <omnetpp.h>

#include "../CloudProviders/CloudProviderFirstFit/NodeResourceInfo.h"
#include "DataCentre.h"
/**
 *
 * Class that contains data structures for monitoring a Data-Centre.
 * This class manages several list of nodes where each list contains those nodes with the same number of CPU cores.
 *
 */
class DataCentre_CpuType{

    private:

        /** Name of the Data-Centre */
        std::string name;

        /** This variable is used to have a quick element to check if the DC is full*/
        int nTotalAvailableCores;

        /** Total number of core of the CPU*/
        int nTotalCores;

        /** List of nodes with 1 CPU core */
        std::vector <NodeResourceInfo*> nodes_1core;
        std::vector <NodeResourceInfo*> free_nodes_1core;

        /** List of nodes with 2 CPU cores */
        std::vector <NodeResourceInfo*> nodes_2cores;
        std::vector <NodeResourceInfo*> free_nodes_2cores;

        /** List of nodes with 4 CPU cores */
        std::vector <NodeResourceInfo*> nodes_4cores;
        std::vector <NodeResourceInfo*> free_nodes_4cores;

        /** List of nodes with 8 CPU cores */
        std::vector <NodeResourceInfo*> nodes_8cores;
        std::vector <NodeResourceInfo*> free_nodes_8cores;

        /** List of nodes with 16 CPU cores */
        std::vector <NodeResourceInfo*> nodes_16cores;
        std::vector <NodeResourceInfo*> free_nodes_16cores;

        /** List of nodes with 32 CPU cores */
        std::vector <NodeResourceInfo*> nodes_32cores;
        std::vector <NodeResourceInfo*> free_nodes_32cores;

        /**
         * Get a node list taking into account an specific number of cores.
         * @param numCores Number of cores.
         * @return List of nodes.
         */
        std::vector <NodeResourceInfo*>* getNodeListByCores(int numCores);
        /**
         * Get a free node list taking into account an specific number of cores.
         * @param numCores Number of cores.
         * @return List of nodes.
         */
        std::vector <NodeResourceInfo*>* getFreeNodeListByCores(int numCores);
        /**
         * Allocs the resources necessaries to handle a given VM request.
         * @param pVmRequest VM Resource request.
         * @param pList The list where the node will be allocated.
         * @return The node where the resources will be allocated.
         */
        NodeResourceInfo* allocNewResources(NodeResourceRequest* pVmRequest, std::vector <NodeResourceInfo*>*& pList);

    public:

       /**
        * Constructor.
        */
        DataCentre_CpuType(std::string name);

       /**
        * Destructor.
        */
        virtual ~DataCentre_CpuType();

       /**
        * Gets the name of this data-centre.
        *
        * @return Name of this data-centre.
        */
        const std::string& getName() const;

        /**
         * Inserts a new node into the corresponding vector.
         *
         * @param newNode Node to be included.
         * @param numCpus Number of CPUs.
         */
        void insertNode (NodeResourceInfo* newNode, int numCpus);

        /**
         * Get the total number of available cores of the datacentre
         * @return Number of available cores
         */
        int getTotalAvailableCores(){return nTotalAvailableCores;};
        /**
         * Get the total number of total cores of the datacentre
         * @return Number of total cores
         */
        int getTotalCores(){return nTotalCores;};
        /**
         * Handle a VM request
         * @param pVmRequest VM request
         * @return The node where the resources will be allocated
         */
        NodeResourceInfo* handleVmRequest(NodeResourceRequest* pVmRequest);
        /**
         * Free the resources of a VM.
         * @param strVmId Identifier of the VM.
         * @param pResInfo The node where the resources are allocated.
         */
        void freeVmFromNode(std::string strVmId, NodeResourceInfo* pResInfo);
        /**
         * Provides information about the available resources of the datacentre.
         * @return String with the information of the datacentre
         */
        std::string getAvailableResources();
        /**
         * Find a node in a list.
         * @param pList The list where the node will be searched.
         * @param strResourceId Identifier of the node.
         * @return The index of the node, -1 if the node is not located in the list.
         */
        int findResourceIndex(std::vector <NodeResourceInfo*>*& pList, std::string strResourceId);
};

#endif /* DATACENTRE_H_ */
