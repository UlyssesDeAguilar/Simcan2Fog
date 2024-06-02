#ifndef SIMCAN_EX_UDP_IO_SERVICE
#define SIMCAN_EX_UDP_IO_SERVICE

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/socket/SocketMap.h"
#include "Architecture/Network/DNS/common.h"
#include "Architecture/Network/Stack/NetworkIOEvent_m.h"
#include "Architecture/Network/Stack/StackMultiplexer.h"
#include "Messages/INET_AppMessage.h"

using namespace inet;

class UdpIoService : public ApplicationBase, public UdpSocket::ICallback, public StackService
{
protected:
    struct VmSocketBinding
    {
        uint32_t vmId;
        uint32_t pid;
        uint32_t ip;
    };

    std::map<int, VmSocketBinding> bindingMap;
    SocketMap socketMap;
    StackMultiplexer *multiplexer; // For forwarding the responses from the endpoints

    // Kernel lifecycle
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    // INET lifecyle
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;

    // Socket callbacks
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;
    UdpSocket* setUpNewSocket(networkio::CommandEvent *event);
public:
    virtual void processRequest(cMessage *msg) override;
};

#endif