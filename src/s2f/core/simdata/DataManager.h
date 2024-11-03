#ifndef SIMCAN_EX_DATA_MANAGER
#define SIMCAN_EX_DATA_MANAGER

#include <omnetpp.h>
#include <memory>
#include "DataRepository.h"
#include "s2f/core/simdata/engine/DataEngine.h"

/**
 * @brief This is a manager that provides an interface (facade)
 * to the ecosystem of data sources like SimSchema.
 * It also caches the queried results for efficient lookups.
 * @details It uses a hash map as an optimistic cache,
 * in the future it should be considered to add more sensible approaches
 * @author Ulysses de Aguilar
 * @version 0.1
 */
class DataManager : public omnetpp::cSimpleModule
{
private:
    std::string experiment; // The name or "id" of the experiment to be run
    s2f::data::DataEngine *engine{};
    DataRepository *repository{};

protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override { error("This class doesn't recieve messages"); }
    virtual void finish() override;
public:
    /**
     * @brief Searches the Application
     *
     * @param name Name of the application to search
     * @return Application or nullptr if there was an error
     */
    const Application *searchApp(const std::string &name);

    /**
     * @brief Searches the Virtual Machine
     *
     * @param name Name of the virtual machine to search
     * @return VirtualMachine or nullptr if there was an error
     */
    const VirtualMachine *searchVm(const std::string &name);

    /**
     * @brief Searches the SLA
     *
     * @param name Name of the application to search
     * @return Sla or nullptr if there was an error
     */
    const Sla *searchSla(const std::string &name);

    /**
     * @brief Searches the specified user
     *
     * @param userType Name of the user type
     * @return CloudUserPriority or nullptr if there was an error
     */
    const CloudUserPriority *searchUser(const std::string &userType);

    /**
     * @brief Returns a vector with the user requested types
     * @details They're cached after loading
     * @throw cRuntimeError if for some reason the user vector list could not be loaded.
     * @return Vector of CloudUserPriority
     */
    std::unique_ptr<std::vector<const CloudUserPriority *>> getSimUsers();
};

#endif /* SIMCAN_EX_DATA_MANAGER*/