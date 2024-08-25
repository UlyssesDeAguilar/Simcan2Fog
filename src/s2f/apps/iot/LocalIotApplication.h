#ifndef __LOCAL_IOT_APPLICATION_H_
#define __LOCAL_IOT_APPLICATION_H_

#include <omnetpp.h>

#include "s2f/architecture/nodes/iot/IotPayload_m.h"
#include "s2f/apps/AppBase.h"

class LocalIotApplication : public AppBase, public AppBase::ICallback
{
protected:
    unsigned int processingMIs;       //!< Number of MIs to be executed
    //const char *endpointName{};       //!< Domain of the endpoint
    uint32_t endpointIp{};            //!< Remote endpoint ip
    int udpSocket;                    //!< UdpSocket
    int16_t listeningPort;            //!< Port where to listen
    std::vector<L3Address> actuators; //!< Actuators ips
    simtime_t simStartTime;           //!< Simulation Starting timestamp
    time_t runStartTime;              //!< Running starting timestamp (real execution time)

    bool serviceUp = false;
    int numPings = 0;

    virtual void processSelfMessage(cMessage *msg) override;
    virtual void initialize() override;
    virtual void finish() override;

    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override;
    virtual void returnRead(simtime_t timeElapsed) override;
    virtual void returnWrite(simtime_t timeElapsed) override;
    virtual void handleDataArrived(int sockFd, Packet *p) override;
    virtual void handleConnectReturn(int sockFd, bool connected) override;
    virtual bool handlePeerClosed(int sockFd) override;
    virtual void handleResolutionFinished(const L3Address ip, bool resolved) override { error("LocalIotApp: no resolving"); }
};

#endif