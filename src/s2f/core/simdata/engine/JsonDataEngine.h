/**
 * @file SimSchema.cpp
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines the module that implements the SimSchema interface with a MySQL database
 * @version 0.1
 * @date 2023-12-28
 *
 */

#ifndef SIMCAN_EX_SIM_SCHEMA_JSON
#define SIMCAN_EX_SIM_SCHEMA_JSON

// Interface contract
#include <cstdlib>

// Allow lambdas
#include <functional>

// OMNeT++ framework
#include <omnetpp.h>
#include "s2f/core/simdata/engine/DataEngine.h"

namespace s2f
{
    namespace data
    {
        /**
         * @author Ulysses de Aguilar Gudmundsson
         * @brief SimSchema implementation with JSON-like definitions
         */
        class JsonDataEngine : public DataEngine
        {
        private:
            template <class T>
            using DefinitionMap = std::map<omnetpp::opp_string, T>;

        protected:
            virtual void initialize() override;
            virtual void finish() override;
            virtual void handleMessage(omnetpp::cMessage *msg) override;

            /**
             * @brief Load applications from JSON definitions in the 'apps' parameter
             */
            virtual void loadApps();

            /**
             * @brief Load VMs from the JSON definitions in the 'vms' parameter
             */
            virtual void loadVms();

            /**
             * @brief Load SLAs from JSON definitions in the 'slas' parameter
             * 
             * @param checkConsistency Flag to check if the VMs referenced in the SLAs are
             * present in the repository
             */
            virtual void loadSlas(bool checkConsistency);

        public:
            virtual Application *searchApp(const std::string &name) override;
            virtual VirtualMachine *searchVm(const std::string &name) override;
            virtual Sla *searchSla(const std::string &name) override;
            virtual CloudUserPriority *searchUser(const std::string &userType, const std::string &experiment) override;
            virtual void loadUsersInExperiment(const std::string &experiment) override;
        };
    };
};

#endif /*SIMCAN_EX_SIM_SCHEMA_JSON*/
