#ifndef SIMCAN_EX_DNS_SERVICE
#define SIMCAN_EX_DNS_SERVICE

#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"
#include "inet/networklayer/common/L3Address.h"
#include "Architecture/Network/DNS/common.h"


/**
 * @brief Class that provides the DNS service
 * It relies on the XML dump from the Ipv4Configurator
 */
namespace dns
{
    using namespace inet;
    class DNS_Service : public ApplicationBase, UdpSocket::ICallback
    {

    private:
        // typedef std::map<L3Address, std::string&> IpNameMap; Could be used to do reverse resolutions

        static std::set<std::string> prefixSet; //= {"dc", "fg", "ed", "cloudProvider", "userGenerator"};

        NameIpMap records;
        const char *config_file;
        bool isMain; // Indicates wheter it is the root DNS or not !
        bool debug;  // Wheter to print or not the obtained debug information

        void processXMLInterface(cXMLElement *elem);
        bool filterHostByName(std::string hostName);

    protected:
        typedef DNS_Request *(DNS_Service::*Handler_t)(const DNS_Request *);
        UdpSocket socket;

        // Debug utility
        void printRecords();

        // Kernel lifecycle
        virtual int numInitStages() const override { return NUM_INIT_STAGES + 1; }
        virtual void initialize(int stage) override;
        virtual void finish() override;

        // INET lifecyle
        virtual void handleStartOperation(LifecycleOperation *operation) override;
        virtual void handleStopOperation(LifecycleOperation *operation) override { error("This module doesn't support stopping"); }
        virtual void handleCrashOperation(LifecycleOperation *operation) override { error("This module doesn't support crashing"); }

        // Logic
        virtual void handleMessageWhenUp(cMessage *msg) override { socket.processMessage(msg); }
        DNS_Request *selectAndExecHandler(const DNS_Request *request);
        DNS_Request *handleInsert(const DNS_Request *request);
        DNS_Request *handleQuery(const DNS_Request *request);
        DNS_Request *handleDelete(const DNS_Request *request);
        DNS_Request *handleNotImplemented(const DNS_Request *request);

        // Socket callbacks
        virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
        virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
        virtual void socketClosed(UdpSocket *socket) override {} // Ignored, as it doesn't require any action
    };
}

#endif
