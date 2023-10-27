#ifndef SIMCAN_EX_DNS_RESOLVER
#define SIMCAN_EX_DNS_RESOLVER

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/networklayer/common/L3Address.h"

using namespace inet;

class DnsResolver : public ApplicationBase, UdpSocket::ICallback
{
protected:
    UdpSocket socket;

    // Kernel lifecycle
    virtual int numInitStages() const override {return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    // INET lifecyle
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    // Logic
    virtual void handleMessageWhenUp(cMessage *msg) override;

    // Socket callbacks
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
};

#endif

/**
 *  Debería facilitar la siguiente API para las Apps ! (Mover luego porfa)
     * @brief Resolves the requested address given a type and a name
     * @details If you select the CLOUD_PROVIDER the name will be ignored as there is only one.
     * If you select the other two kinds it will search for the name provided, composed of the 
     * complete path or only the module name.
     * @param type The type of the module that we will try to access
     * @param name The name of the module (full path or the name)
     * @return inet::L3Address The resolved Ip Address
    inet::L3Address resolve(DNS_Type type, const std::string &name) const;

    /**
     * @brief Similar to the resolve method, but instead it returns 
     * all of the addresses of particular kind (designed for the CloudProvider) 
     * 
     * @param type The type of addresses to be resolved
     * @return std::vector<inet::L3Address> Constant vector that contains the Ip Addresses 
    const std::vector<inet::L3Address>& resolveAll(DNS_Type type) const;
*/