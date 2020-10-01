#ifndef __SIMCAN_2_0_DATACENTERMANAGER_H_
#define __SIMCAN_2_0_DATACENTERMANAGER_H_

#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/dataClasses/CloudUserInstance.h"
#include "Management/parser/DataCenterConfigParser.h"
#include "OperatingSystem/Hypervisors/Hypervisor/Hypervisor.h"

/**
 * Module that implementa a data-center manager for cloud environments
 */
class DataCenterManager: public CloudManagerBase{
public:
    int getNTotalAvailableCores() const;
    void setNTotalAvailableCores(int nTotalAvailableCores);
    int getNTotalCores() const;
    void setNTotalCores(int nTotalCores);

    protected:

        /** Show information of DataCenters */
        bool showDataCenterConfig;

        int nTotalCores;

        int nTotalAvailableCores;

        /** Users */
        std::vector<CloudUserInstance*> users;

        /** Input gate from DataCenter. */
        cGate* inGate;

        /** Output gates to DataCenter. */
        cGate* outGate;

        /** Vector that contains a collection of structures for monitoring data-centers */
        std::vector<DataCenter*> dataCentersBase;

        std::map<int, std::vector<Hypervisor*>> mapHypervisorPerNodes;

        ~DataCenterManager();
        virtual void initialize();

        int initDataCenterMetadata();


       /**
        * Parsea el parametro con la lista de aplicaciones al vector de aplicaciones
        */
        int parseDataCenterConfig();

       /**
        * String que contiene todas los data centers
        */
        std::string dataCenterToString();

       /**
        * Get the out Gate to the module that sent <b>msg</b>.
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg);

       /**
        * Process a self message.
        * @param msg Self message.
        */
        virtual void processSelfMessage (cMessage *msg);

       /**
        * Process a request message.
        * @param sm Request message.
        */
        virtual void processRequestMessage (SIMCAN_Message *sm);

       /**
        * Process a response message from a module in the local node.
        * @param sm Response message.
        */
        virtual void processResponseMessage (SIMCAN_Message *sm);

        virtual int storeNodeMetadata(cModule *pNodeModule);

        virtual void parseConfig() override;

        virtual void handleVmRequestFits(SIMCAN_Message *sm);

        bool checkVmUserFit(SM_UserVM*& userVM_Rq);
};

#endif
