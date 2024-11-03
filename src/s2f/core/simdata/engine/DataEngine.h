/**
 * @file SimSchema.cpp
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines the interface for the module that will allow to retrieve details about
 * composition and parameters of the entities involved in the simulation.
 * @version 0.1
 * @date 2023-12-28
 *
 */

#ifndef SIMCAN_EX_SIM_SCHEMA
#define SIMCAN_EX_SIM_SCHEMA

// C++ 11 utilities
#include <memory>
#include <stdlib.h>

#include "s2f/core/simdata/DataRepository.h"

// Data Classes that represent the information
#include "s2f/management/dataClasses/Applications/Application.h"
#include "s2f/management/dataClasses/SLAs/Sla.h"
#include "s2f/management/dataClasses/Users/CloudUser.h"
#include "s2f/management/dataClasses/Users/CloudUserPriority.h"
#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"

namespace s2f
{
    namespace data
    {
        /**
         * @author Ulysses de Aguilar
         * @brief Interface for retrieving simulation information
         */
        class DataEngine : public omnetpp::cSimpleModule
        {
        protected:
            DataRepository *repository;

            virtual void initialize() override;
            //virtual void finish() override;
            virtual void handleMessage(omnetpp::cMessage *msg) override;

        public:
            /**
             * @brief Retrieves and builds the Application data class
             *
             * @param name Name of the application to search
             * @return Application or nullptr if there was an error
             */
            virtual Application *searchApp(const std::string &name) = 0;

            /**
             * @brief Retrieves and builds the Virtual Machine data class
             *
             * @param name Name of the virtual machine to search
             * @return VirtualMachine or nullptr if there was an error
             */
            virtual VirtualMachine *searchVm(const std::string &name) = 0;

            /**
             * @brief Retrieves and builds the Sla data class
             *
             * @param name Name of the application to search
             * @return Sla or nullptr if there was an error
             */
            virtual Sla *searchSla(const std::string &name) = 0;

            /**
             * @brief Retrieves in a list the requested user templates for a given experiment
             *
             * @param experiment The name of the experiment
             * @return Vector of strings or nullptr if there was an error
             */
            virtual CloudUserPriority *searchUser(const std::string &userType, const std::string &experiment) = 0;

            /**
             * @brief Retrieves in a list the requested user templates for a given experiment
             *
             * @param experiment The name of the experiment
             * @return Vector of strings or nullptr if there was an error
             */
            virtual void loadUsersInExperiment(const std::string &experiment) = 0;
        };
    };
};

#endif /*SIMCAN_EX_SIM_SCHEMA*/
