#ifndef SIMCAN_EX_DNS_SERVICE
#define SIMCAN_EX_DNS_SERVICE

#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/networklayer/common/L3Address.h"
#include "Messages/INET_AppMessage.h"

using namespace omnetpp;
using namespace inet;

/**
 * @brief Class that provides the DNS service
 * It relies on the XML dump from the Ipv4Configurator
 */
class DNS_Service : public ApplicationBase, UdpSocket::ICallback
{
private:
    typedef std::map<std::string, L3Address> NameIpMap;
    // typedef std::map<L3Address, std::string&> IpNameMap; Could be used to do reverse resolutions

    static std::set<std::string> prefixSet; //= {"dc", "fg", "ed", "cloudProvider", "userGenerator"};

    NameIpMap records;
    bool isMain; // Indicates wheter it is the root DNS or not !

    void processXMLInterface(cXMLElement *elem);
    bool filterHostByName(std::string hostName);

protected:
    UdpSocket socket;

    // Kernel lifecycle
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    // INET lifecyle
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
    virtual void handleCrashOperation(LifecycleOperation *operation) override { error("This module doesn't support crashing"); }

    // Logic
    virtual void handleMessageWhenUp(cMessage *msg) override { socket.processMessage(msg); }

    // Socket callbacks
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override {}    // Ignored, as it doesn't require any action
};

#endif
