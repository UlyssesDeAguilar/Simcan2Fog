#ifndef __IOT_APPLICATION_H_
#define __IOT_APPLICATION_H_

#include <omnetpp.h>

#include "../../../architecture/nodes/iot/IotPayload_m.h"
#include "s2f/apps/models/AppBase.h"

class IotApplication : public AppBase, public AppBase::ICallback
{
protected:
    unsigned int processingMIs;       //!< Number of MIs to be executed
    const char *endpointName{};       //!< Domain of the endpoint
    Ipv4Address endpointIp{};         //!< Remote endpoint ip
    int udpSocket;                    //!< UdpSocket
    int tcpSocket;                    //!< TcpSocket
    double chronometer;                  //!< Measures round trip time
    int16_t listeningPort;            //!< Port where to listen
    std::vector<L3Address> actuators; //!< Actuators ips
    simtime_t simStartTime;           //!< Simulation Starting timestamp
    time_t runStartTime;              //!< Running starting timestamp (real execution time)

    bool serviceUp = false;
    int numPings = 0;

    void sendServer();
    void sendActuator();

    virtual void processSelfMessage(cMessage *msg) override;
    virtual void initialize() override;
    virtual void finish() override;
    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override;
    virtual void returnRead(simtime_t timeElapsed) override {}
    virtual void returnWrite(simtime_t timeElapsed) override {}
    virtual void handleDataArrived(int sockFd, Packet *p) override;
    virtual void handleConnectReturn(int sockFd, bool connected) override;
    virtual bool handlePeerClosed(int sockFd) override;
    virtual void handleResolverReturned(uint32_t ip, bool resolved) override;
};

#endif
