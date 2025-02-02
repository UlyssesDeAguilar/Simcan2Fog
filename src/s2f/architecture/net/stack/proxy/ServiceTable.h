#ifndef SIMCAN_EX_SERVICE_TABLE
#define SIMCAN_EX_SERVICE_TABLE

#include "omnetpp.h"

/**
 * @struct ServiceEntry
 * @brief Represents a service entry in a service table.
 */
struct ServiceEntry
{
    /**
     * @union Id
     * @brief Union that contains two different representations of a service entry.
     */
    union Id
    {
        uint64_t id;         //!< Unique identifier for the service entry.
        uint32_t address[2]; //!< IP address and port of the service entry.
    } entry;

    /**
     * @brief Constructor that initializes the service entry with the provided IP and port.
     * @param ip The IP address of the service entry.
     * @param port The port of the service entry.
     */
    ServiceEntry(uint32_t ip, uint32_t port) : entry{.address = {ip, port}} {}

    /**
     * @brief Get the IP address of the service entry.
     * @return The IP address of the service entry.
     */
    uint32_t getIp() const { return entry.address[0]; }

    /**
     * @brief Get the port of the service entry.
     * @return The port of the service entry.
     */
    uint32_t getPort() const { return entry.address[1]; }

    /**
     * @brief Overloaded less-than operator that compares the service entries based on their IDs.
     * @param other The other service entry to compare with.
     * @return True if the ID of this service entry is less than the ID of the other service entry, false otherwise.
     */
    bool operator<(const ServiceEntry &other) const { return entry.id < other.entry.id; }
};

/**
 * @class ServiceTable
 * @brief Represents a service table that stores service entries.
 */
class ServiceTable : public omnetpp::cSimpleModule
{
public:
    /**
     * @typedef ServiceTableMap
     * @brief Type alias for a map of service names to service entries.
     */
    using ServiceTableMap = std::map<omnetpp::opp_string, std::vector<ServiceEntry>>;

    /**
     * @class IServiceTableSubscriber
     * @brief Interface for objects that want to subscribe to service table events.
     */
    class IServiceTableSubscriber
    {
    public:
        /**
         * @brief Called when a service is registered for the first time.
         * @param serviceName The name of the registered service.
         */
        virtual void serviceRegistered(const char *serviceName) = 0;

        /**
         * @brief Called when a service is unregistered as there are no entries left.
         * @param serviceName The name of the unregistered service.
         */
        virtual void serviceUnregistered(const char *serviceName) = 0;
    };

    /**
     * @brief Subscribe to service table events.
     * @param observer The object that wants to subscribe to events.
     */
    void subscribeToUpdates(IServiceTableSubscriber *observer) { subscribers.insert(observer); }

    /**
     * @brief Unsubscribe from service table events.
     * @param observer The object that wants to unsubscribe from events.
     */
    void unsubscribeFromUpdates(IServiceTableSubscriber *observer) { subscribers.erase(observer); }

    /**
     * @brief Register a service in the service table.
     * @param serviceName The name of the service to register.
     * @param ip The IP address of the service.
     * @param port The port of the service.
     */
    void registerService(const char *serviceName, int ip, int port);

    /**
     * @brief Unregister a service from the service table.
     * @param serviceName The name of the service to unregister.
     * @param ip The IP address of the service.
     * @param port The port of the service.
     */
    void unregisterService(const char *serviceName, int ip, int port);

    /**
     * @brief Find a service in the service table.
     * @param serviceName The name of the service to find.
     * @return An iterator pointing to the service entry, or the end of the map if not found.
     */
    ServiceTableMap::iterator findService(const char *serviceName) { return serviceMap.find(serviceName); }

    /**
     * @brief Get the end of the service table map.
     * @return An iterator pointing to the end of the map.
     */
    ServiceTableMap::iterator endOfServiceMap() { return serviceMap.end(); }

    /**
     * @brief Get the domain where this services reside
     * @return const char* The domain
     */
    const char * getDomain() const { return domain; }

protected:
    ServiceTableMap serviceMap;                      //!< Map of service names to service entries.
    std::set<IServiceTableSubscriber *> subscribers; //!< The set of subscribers to service table events.
    const char *domain;                              //!< The domain of the services contained in the table.

    /**
     * @brief Initialize the service table.
     */
    void initialize() override;

    /**
     * @brief Does the cleanup in order to finish the simulation
     */
    void finish() override;

    /**
     * @brief Handle a message (not implemented).
     * @param msg The message to handle.
     */
    void handleMessage(omnetpp::cMessage *msg) override { error("This module doesn't take messages"); }
};

#endif /* SIMCAN_EX_SERVICE_TABLE */
