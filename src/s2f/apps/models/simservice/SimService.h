#ifndef SIMCAN_EX_PROCESSING_SERVICE
#define SIMCAN_EX_PROCESSING_SERVICE

#include "s2f/core/simdata/DataManager.h"
#include "s2f/management/dataClasses/VirtualMachines/VirtualMachine.h"
#include "s2f/management/managers/ResourceManager.h"
//#include "s2f/management/DataCentreManagers/Selection/Strategies.h"
#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

using namespace omnetpp;
using namespace inet;

class SimService : public cSimpleModule, public LifecycleUnsupported
{
  protected:
    TcpSocket socket;
    simtime_t delay;
    simtime_t maxMsgDelay;

    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;

    std::map<int, ChunkQueue> socketQueue;
    ResourceManager *resourceManager;
    DataManager *dataManager;        
  protected:
    virtual void sendBack(cMessage *msg);
    virtual void sendOrSchedule(cMessage *msg, simtime_t delay);
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
};

#endif