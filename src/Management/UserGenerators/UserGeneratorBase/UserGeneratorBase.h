#ifndef __SIMCAN_2_0_USERGENERATOR_BASE_H_
#define __SIMCAN_2_0_USERGENERATOR_BASE_H_

#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/dataClasses/CloudUserInstance.h"
#include "Messages/SM_UserVM_m.h"

/**
 * Base class for User generators.
 *
 * This class parses and manages cloud users
 */
class UserGeneratorBase: public CloudManagerBase{

    protected:

        /** Vector that contains the user instances */
        std::vector<std::vector <CloudUserInstance*>> groupOfUsers;

        /** Vector that contains all the cloud (individual) user instances */
        std::vector<CloudUserInstance*> userInstances;

        /** */
        int nUserInstancesFinished;

        /**Index of the next user which must be processed */
        int nUserIndex;

        /**Hasmap to accelerate the management of the users*/
        std::map<std::string, CloudUserInstance*> userHashMap;

        /** Input gate from DataCenter. */
        cGate* fromCloudProviderGate;

        /** Output gates to DataCenter. */
        cGate* toCloudProviderGate;

        /** Show parsed user instances */
        bool showUserInstances;

        /** Flag for generating all the user instances at once */
        bool intervalBetweenUsers;

        bool shuffleUsers;

        /**< Starting time delay */
        double startDelay;

        /** Interval gap between users arrivals */
        cPar *distribution;

        /**
         * Destructor
         */
        ~UserGeneratorBase();

        /**
         * Initialize method. Invokes the parsing process to allocate the existing cloud users in the corresponding data structures.
         */
        virtual void initialize();


       /**
        * Get the out Gate to the module that sent <b>msg</b>.
        *
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or \a nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg);

       /**
        * Generates the collection of user instances that were defined in the initial configuration.
        *
        * This process is performed before the simulation starts (in cold).
        *
        */
        virtual void generateUsersBeforeSimulationStarts ();

        /**
         * Parses the current content of the users vector into a string.
         *
         * @return String containing the current status of the users vector.
         */
        virtual string usersIstancesToString ();

        virtual CloudUserInstance* createCloudUserInstance(CloudUser *ptrUser, unsigned int  totalUserInstance, unsigned int  userNumber, int currentInstanceIndex, int totalUserInstances);

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
