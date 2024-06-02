#ifndef __LOCAL_IOT_APPLICATION_H_
#define __LOCAL_IOT_APPLICATION_H_

#include <omnetpp.h>
#include "Applications/Base/UserAppBase/UserAppBase.h"

class LocalIotApplication : public UserAppBase, public UserAppBase::ICallback
{
protected:
    unsigned int processingMIs;      //!< Number of MIs to be executed
    const char *endpointName{};      //!< Domain of the endpoint
    uint32_t endpointIp{};           //!< Remote endpoint ip
    int udpSocket;                   //!< UdpSocket
    int16_t listeningPort;           //!< Port where to listen
    std::vector<uint32_t> actuators; //!< Actuators ips
    simtime_t simStartTime;          //!< Simulation Starting timestamp
    time_t runStartTime;             //!< Running starting timestamp (real execution time)

    virtual void initialize() override;
    virtual void finish() override;
    virtual bool run() override;
};

#endif