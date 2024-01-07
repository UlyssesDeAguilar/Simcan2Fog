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
    virtual std::unique_ptr<Application> searchApp(const std::string &name) = 0;

    /**
     * @brief Retrieves and builds the Virtual Machine data class
     *
     * @param name Name of the virtual machine to search
     * @return VirtualMachine or nullptr if there was an error
     */
    virtual std::unique_ptr<VirtualMachine> searchVirtualMachine(const std::string &name) = 0;

    /**
     * @brief Retrieves and builds the Sla data class
     *
     * @param name Name of the application to search
     * @return Sla or nullptr if there was an error
     */
    virtual std::unique_ptr<Sla> searchSLA(const std::string &name) = 0;

    /**
     * @brief Retrieves and builds the VMCost data class based on a sla and virtual machine type
     *
     * @param sla Name of the sla to search
     * @param vmType Kind of vm to search
     * @return Sla::VMCost or nullptr if there was an error
     */
    virtual std::unique_ptr<Sla::VMCost> searchVMCost(const std::string &sla, const std::string &vmType) = 0;
};

#endif /*SIMCAN_EX_SIM_SCHEMA*/
