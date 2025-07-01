#ifndef __SIMCAN_2_0_USERGENERATOR_BASE_H_
#define __SIMCAN_2_0_USERGENERATOR_BASE_H_

#include <fstream>
#include "s2f/core/include/GroupVector.hpp"
#include "s2f/messages/SM_UserVM.h"

#include "s2f/management/dataClasses/Users/CloudUserInstance.h"
#include "s2f/management/dataClasses/Users/CloudUserInstanceTrace.h"
#include "s2f/management/managers/ManagerBase.h"

namespace s2f
{
    namespace users
    {
        /**
         * @brief Base class for User generators.
         * @details Class parses and manages cloud users
         *
         * @author Pablo Cerro Cañizares
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.3
         */
        class UserGeneratorBase : public ManagerBase
        {

        protected:
            typedef std::map<opp_string, CloudUserInstance *> UserMap;
            typedef std::vector<CloudUserInstance *> UserInstanceList;

            std::vector<std::vector<CloudUserInstance *>> groupOfUsers; //!< Vector that contains the user instances
            UserInstanceList userInstances;                             //!< Vector that contains all the cloud (individual) user instances
            UserMap userHashMap;                                        //!< Hashmap to accelerate the management of the users (EDIT: It is an RB tree actually)
            int nUserInstancesFinished;                                 //!< Number of user instances that have finished
            int nUserIndex;                                             //!< Index of the next user which must be processed
            cGate *fromCloudProviderGate;                               //!< Input gate from DataCentre
            cGate *toCloudProviderGate;                                 //!< Output gates to DataCentre
            bool showUserInstances;                                     //!< Show parsed user instances
            bool intervalBetweenUsers;                                  //!< Flag for generating all the user instances at once
            bool shuffleUsers;                                          //!< Flag for suffling users
            bool activeCycles;                                          //!< FIXME: Figure out what this does
            VM_Request::InstanceRequestTimes timeoutsTemplate;          //!< The "maximum" times acceptable by users
            const char *strUserTraceFilePath;                           //!< Path of the swf trace file (for job parsing)
            int userTraceMaxVms;                                        //!< Maximum number of vms per user loaded from a trace (?)
            double startDelay;                                          //!< Starting time delay
            cPar *distribution;                                         //!< Distribution that defines the interval gap between users arrivals
            int numberOfCycles;                                         //!< Number of cycles
            bool isolateCycle;                                          //!< Wheter to isolate the next cycle
            double durationOfCycle;                                     //!< Duration of a cycle
            cPar *cycleDistribution;                                    //!< Distribution of the cycles
            int traceStartTime;                                         //!< Start time for reading the traces
            int traceEndTime;                                           //!< End time for reading the traces
            SimTime m_dInitSim;                                         //!< Figure out what this does

            /**
             * Destructor
             */
            ~UserGeneratorBase();

            /**
             * Initialize method. Invokes the parsing process to allocate the existing cloud users in the corresponding data structures.
             */
            virtual void initialize() override;
            virtual void initializeRequestHandlers() override {};

            /**
             * Get the out Gate to the module that sent <b>msg</b>.
             *
             * @param msg Arrived message.
             * @return Gate (out) to module that sent <b>msg</b> or \a nullptr if gate not found.
             */
            virtual cGate *getOutGate(cMessage *msg) override;

            /**
             * Generates the collection of user instances that were defined in the initial configuration.
             *
             * This process is performed before the simulation starts (in cold).
             *
             */
            virtual void generateUsersBeforeSimulationStarts();

            /**
             * Shuffles the arrival time of the generated users
             */
            virtual void generateShuffledUsers();

            virtual void generateUserArrivalTimes();

            /**
             * Parses the current content of the users vector into a string.
             *
             * @return String containing the current status of the users vector.
             */
            virtual std::string usersIstancesToString();

            virtual CloudUserInstance *createCloudUserInstance(const CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances);

            void parseTraceFile();

            /**
             * Builds the VM request which corresponds with the provided user instance.
             */
            virtual SM_UserVM *createVmRequest(CloudUserInstance *pUserInstance);

            /**
             * Returns the time the next user will arrive to the cloud
             *
             * @param pUserInstance Pointer to the user instance.
             * @param last Last user arrival time.
             */
            virtual SimTime getNextTime(CloudUserInstance *pUserInstance, SimTime last);

            virtual SM_UserVM *createVmMessage();

            virtual CloudUserInstance *createNewUserFromTrace(Job_t jobIn, int totalUserInstance, int nCurrentNumber, int nUserInstance, int nTotalInstances);

            virtual CloudUser *createUserTraceType();
        };
    }
}

#endif /*__SIMCAN_2_0_USERGENERATOR_BASE_H_ */
