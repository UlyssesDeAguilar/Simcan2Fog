#ifndef SIMCAN_EX_EDGE_USER_MODEL_DRIVER
#define SIMCAN_EX_EDGE_USER_MODEL_DRIVER

#include "s2f/users/common.h"
#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/socket/SocketMap.h"
#include "s2f/users/edge/EdgeUsersControl_m.h"

using namespace omnetpp;

namespace users
{
    class EdgeUser : public cSimpleModule
    {
    protected:
        std::vector<cModule *> managedSensors;
        std::map<opp_string, users::Vm> managedVms;
        std::vector<users::App> localApps;

        const char *deploymentZone;
        GateInfo serialGates;
        opp_string userId;

        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;

        virtual void startup();
        virtual void handleVmUpdate(SM_UserVM *update);
        virtual void handleAppEvent(SM_UserAPP *update);
        virtual void processSerialEvent(cMessage *msg);

        virtual void turnSensorsOn();
        virtual void turnSensorsOff();
        virtual users::Vm *findPlatform(const char *name);
        opp_string generateUniqueId(const char *name);
        void sendServiceIsUp(const char *localApp, const App &service, const char* zone);
    };
}

#endif