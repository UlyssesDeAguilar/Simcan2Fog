#ifndef SIMCAN_EX_IOT_SENSOR_MODEL_H
#define SIMCAN_EX_IOT_SENSOR_MODEL_H

#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

using namespace omnetpp;

namespace s2f::iot
{
  class IotSensorModel : public inet::ApplicationBase, public inet::UdpSocket::ICallback
  {
  protected:
    enum SelfMsgKinds
    {
      START = 1,
      SEND,
      STOP
    };

    // parameters
    inet::L3Address controllerAddress;
    int localPort = -1, destPort = -1;

    simtime_t startTime;
    simtime_t stopTime;

    const char *tag = nullptr;
    cPar *value = nullptr;
    const char *unit = nullptr;

    // state
    inet::UdpSocket socket;
    cMessage *selfMsg = nullptr;

    // statistic
    int numSent = 0;
    int numReceived = 0;

  protected:
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void sendPacket();
    virtual void processPacket(inet::Packet *msg);

    virtual void processStart();
    virtual void processSend();
    virtual void processStop();

    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

    virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    virtual void socketClosed(inet::UdpSocket *socket) override;

  public:
    virtual ~IotSensorModel();
  };
}

#endif