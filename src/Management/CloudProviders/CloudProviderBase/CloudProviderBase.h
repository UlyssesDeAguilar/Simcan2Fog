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

       /**
        * Gets the out Gate to the module that sent <b>msg</b>.
        *
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or \a nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg) override;

};

#endif
