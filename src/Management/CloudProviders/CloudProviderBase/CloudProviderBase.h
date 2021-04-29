#ifndef __SIMCAN_2_0_CLOUDPROVIDERBASE_H_
#define __SIMCAN_2_0_CLOUDPROVIDERBASE_H_

#include "../../parser/DataCentreListParser.h"
#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"

/**
 * Base class for Cloud Providers.
 *
 * This class parses and manages data-centres.
 *
 */
class CloudProviderBase : public CloudManagerBase{

    protected:

        /** Shows information of Data-Centres */
        bool showDataCentres;

        /** Vector that contains a collection of structures for monitoring data-centres */
        std::vector<DataCentre*> dataCentresBase;

        /** Input gate from DataCentres. */
        cGate** fromDataCentreGates;

        /** Output gate to DataCentres. */
        cGate** toDataCentreGates;

        /** Input gate from UserGenerator module. */
        cGate* fromUserGeneratorGate;

        /** Output gate to UserGenerator module. */
        cGate* toUserGeneratorGate;

        /** Vector that contains references to data center managers **/
       DataCentreManagerBase** dataCentreManagers;

        /**
         * Destructor.
         */
        ~CloudProviderBase();

        /**
         * Initialize method. Invokes the parsing process to allocate the existing data-centres in the corresponding data structures.
         * Also, this method sets the output gates with the existing data-centres.
         */
        virtual void initialize();

        /**
         * Parses each data-centre configuration used in the simulation.
         * These configurations of data-centres are allocated in the <b>dataCentres</b> vector.
         *
         * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
         */
        virtual int parseDataCentresList();

        /**
         * Converts the parsed data-centres to string format. Usually, this method is invoked for debugging/logging purposes.
         *
         * @return String containing the parsed data-centre configurations.
         */
        std::string dataCentresToString();

       /**
        * Gets the out Gate to the module that sent <b>msg</b>.
        *
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or \a nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg) override;

};

#endif
