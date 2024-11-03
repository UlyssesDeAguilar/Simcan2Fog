/*#include "inet/networklayer/common/L3Address.h"
#include "Architecture/Network/Stack/NetworkIOEvent_m.h"
#include "Architecture/Network/Stack/StackServiceType.h"
#include "Messages/SM_REST_API_m.h"
#include "Messages/SIMCAN_Message.h"
#include <omnetpp.h>

class PingStackDriver : public omnetpp::cSimpleModule
{
private:
    const char *serverAddress; //!< Address of the server (ipv4)
    int numPings;              //!< Number of pings to send
    bool server;               //!< If it's on server mode
    networkio::Event *mt{};    //!< Message template
    SM_REST_API *package{};    //!< Data to be sent
protected:
    virtual ~PingStackDriver();
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
    void handleEvent(cMessage *msg);
};*/