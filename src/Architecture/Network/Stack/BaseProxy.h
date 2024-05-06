#ifndef SIMCAN_EX_BASE_PROXY
#define SIMCAN_EX_BASE_PROXY

#include <algorithm>

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "Architecture/Network/DNS/Service/DNS_Service.h"
#include "Messages/SIMCAN_Message.h"
#include "Messages/INET_AppMessage.h"
#include "StackMultiplexer.h"
#include "NetworkIOEvent_m.h"

/**
 * @brief Holds together the context necessary for relaying comms between TcpBaseThread and the Hypervisor
 * @details This approach assumes that the apps are single-threaded, single-process
 */
struct ServiceEntry
{
    uint32_t pid{};        //!< Process Id
    uint32_t vmId{};       //!< VM Id
    uint32_t ip{};         //!< Ip of the node that host the service
    uint8_t available = 1; //!< If the node is available

    friend bool operator<(const ServiceEntry &a, const ServiceEntry &b)
    {
        return ((a.pid - b.pid) + (a.vmId - b.vmId) << 1 + (a.ip - b.ip) << 2) > 0;
    }
    friend bool operator==(const ServiceEntry &a, const ServiceEntry &b)
    {
        return ((a.pid - b.pid) + (a.vmId - b.vmId) + (a.ip - b.ip)) == 0;
    }
};

class BaseProxy : public inet::ApplicationBase, public StackService
{
private:
    dns::DNS_Request dnsTemplate{};
    std::set<ServiceEntry> &findOrRegisterService(const std::string &service, const uint32_t serviceIp);
    void removeFromPool(const std::string &service, const ServiceEntry &oldEntry);

protected:
    using ServicePoolMap = std::map<std::string, std::set<ServiceEntry>>;

    ServicePoolMap servicePoolMap; //!< Maps services to their pools
    StackMultiplexer *multiplexer; //!< For registering/deleting services on command
    dns::DNS_Service *localDns;    //!< For registering/deleting DNS entries

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }

public:
    // Child classes should let the parent implementation run first
    virtual void processRequest(omnetpp::cMessage *msg) override;
};

#endif