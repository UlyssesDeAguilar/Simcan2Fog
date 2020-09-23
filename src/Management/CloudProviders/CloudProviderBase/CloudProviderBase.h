#ifndef __SIMCAN_2_0_CLOUDPROVIDERBASE_H_
#define __SIMCAN_2_0_CLOUDPROVIDERBASE_H_

#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/parser/DataCenterListParser.h"

/**
 * Base class for Cloud Providers.
 *
 * This class parses and manages data-centers.
 *
 */
class CloudProviderBase : public CloudManagerBase{

    protected:

        /** Shows information of Data-Centers */
        bool showDataCenters;

//        /** Vector that contains a collection of configurations for computing nodes  */
//        std::vector<NodeInfo*> computingNodeTypes;
//
//        /** Vector that contains a collection of configurations for storage nodes */
//        std::vector<NodeInfo*> storageNodeTypes;
//
//        /** Vector that contains a collection of configurations for computing racks */
//        std::vector<RackInfo*> computingRackTypes;
//
//        /** Vector that contains a collection of configurations for storage racks */
//        std::vector<RackInfo*> storageRackTypes;

        /** Vector that contains a collection of structures for monitoring data-centers */
        std::vector<DataCenter*> dataCentersBase;

        /** Input gate from DataCenters. */
        cGate** fromDataCenterGates;

        /** Output gate to DataCenters. */
        cGate** toDataCenterGates;

        /** Input gate from UserGenerator module. */
        cGate* fromUserGeneratorGate;

        /** Output gate to UserGenerator module. */
        cGate* toUserGeneratorGate;



        /**
         * Destructor.
         */
        ~CloudProviderBase();

        /**
         * Initialize method. Invokes the parsing process to allocate the existing data-centers in the corresponding data structures.
         * Also, this method sets the output gates with the existing data-centers.
         */
        virtual void initialize();

        /**
         * Parses each data-center configuration used in the simulation.
         * These configurations of data-centers are allocated in the <b>dataCenters</b> vector.
         *
         * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
         */
        virtual int parseDataCentersList();

        /**
         * Converts the parsed data-centers to string format. Usually, this method is invoked for debugging/logging purposes.
         *
         * @return String containing the parsed data-center configurations.
         */
        std::string dataCentersToString();

//        /**
//         * Search for the configuration of the node <b>nodeName</b>.
//         *
//         * @param nodeName Name of the node.
//         * @param isStorage If this parameter is set to <i>true</i>, then <b>nodeName</b> is searched in the vector containing storage nodes.
//         * In other case, <b>nodeName</b> is searched in the vector containing computing nodes.
//         * @return If the requested node is located in the corresponding vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
//         */
//        NodeInfo* findNodeInfo (std::string nodeName, bool isStorage);
//
//        /**
//         * Search for the configuration of the rack <b>rackName</b>.
//         *
//         * @param rackName Name of the rack.
//         * @param isStorage If this parameter is set to <i>true</i>, then <b>rackName</b> is searched in the vector containing storage racks.
//         * In other case, <b>rackName</b> is searched in the vector containing computing racks.
//         * @return If the requested rack is located in the corresponding vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
//         */
//        RackInfo* findRackInfo (std::string rackName, bool isStorage);

       /**
        * Gets the out Gate to the module that sent <b>msg</b>.
        *
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or \a nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg);

       /**
        * Process a self message.
        *
        * @param msg Received (self) message.
        */
        virtual void processSelfMessage (cMessage *msg) = 0;

        /**
        * Process a request message.
        *
        * @param sm Incoming message.
        */
        virtual void processRequestMessage (SIMCAN_Message *sm) = 0;

        /**
        * Process a response message from an external module.
        *
        * @param sm Incoming message.
        */
        virtual void processResponseMessage (SIMCAN_Message *sm) = 0;

};

#endif
