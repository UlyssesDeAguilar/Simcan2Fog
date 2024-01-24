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

// Data Classes that represent the information
#include "Management/dataClasses/Applications/Application.h"
#include "Management/dataClasses/Applications/AppParameter.h"
#include "Management/dataClasses/SLAs/Sla.h"
#include "Management/dataClasses/VirtualMachines/VirtualMachine.h"
#include "Management/dataClasses/Users/CloudUser.h"
#include "Management/dataClasses/Users/CloudUserPriority.h"

namespace simschema
{
    template <class T>
    using DataList = std::unique_ptr<std::vector<T>>;

    struct UserBaseInfo
    {
        std::string userName;
        std::string slaId;
        uint32_t numInstances;
        tPriorityType priority;
    };

    struct UserAppInfo
    {
        std::string appName;
        uint32_t numInstances;
    };

    struct UserVmInfo
    {
        std::string type;
        uint32_t numInstances;
        int nRentTime;
    };

    /**
     * @author Ulysses de Aguilar
     * @brief Interface for retrieving simulation information
     */
    class SimSchema
    {
    public:
        /**
         * @brief Retrieves and builds the Application data class
         *
         * @param name Name of the application to search
         * @return Application or nullptr if there was an error
         */
        virtual std::unique_ptr<Application>
        searchApp(const std::string &name) = 0;

        /**
         * @brief Retrieves and builds the Virtual Machine data class
         *
         * @param name Name of the virtual machine to search
         * @return VirtualMachine or nullptr if there was an error
         */
        virtual std::unique_ptr<VirtualMachine>
        searchVirtualMachine(const std::string &name) = 0;

        /**
         * @brief Retrieves and builds the Sla data class
         *
         * @param name Name of the application to search
         * @return Sla or nullptr if there was an error
         */
        virtual std::unique_ptr<Sla>
        searchSLA(const std::string &name) = 0;

        /**
         * @brief Retrieves and builds the VMCost data class based on a sla and virtual machine type
         *
         * @param sla Name of the sla to search
         * @param vmType Kind of vm to search
         * @return Sla::VMCost or nullptr if there was an error
         */
        virtual std::unique_ptr<Sla::VMCost>
        searchVMCost(const std::string &sla, const std::string &vmType) = 0;

        /**
         * @brief Retrieves in a list the requested user templates for a given experiment
         *
         * @param experiment The name of the experiment
         * @return Vector of strings or nullptr if there was an error
         */
        virtual DataList<UserBaseInfo>
        searchUsersInExperiment(const std::string &experiment) = 0;

        /**
         * @brief Retrieves the basic information for constructing a user type
         *
         * @param experiment The name of the experiment
         * @param user Name of the user type
         * @return UserBaseInfo or nullptr if there was an error
         */
        virtual std::unique_ptr<UserBaseInfo>
        searchUserBaseInfo(const std::string &user, const std::string &experiment) = 0;

        /**
         * @brief Retrieves all the app needed by a user
         *
         * @param user Name of the user type
         * @return Vector of strings or nullptr if there was an error
         */
        virtual DataList<UserAppInfo>
        searchUserApps(const std::string &user) = 0;

        /**
         * @brief Retrieves all the vms needed by a user
         *
         * @param user Name of the user type
         * @return Vector of strings or nullptr if there was an error
         */
        virtual DataList<UserVmInfo>
        searchUserVms(const std::string &user) = 0;
    };
};

#endif /*SIMCAN_EX_SIM_SCHEMA*/
