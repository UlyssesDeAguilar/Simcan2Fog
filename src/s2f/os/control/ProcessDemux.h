#ifndef SIMCAN_EX_PROCESS_DEMUX__
#define SIMCAN_EX_PROCESS_DEMUX__

#include "s2f/core/logic/LogicDemuxHub.h"
#include "s2f/messages/Syscall_m.h"

class PidProcessDemux : public LogicDemuxHub::IDemux
{
    public:
        virtual int chooseIndex(omnetpp::cMessage *msg) override;
};

class VmIdProcessDemux : public LogicDemuxHub::IDemux
{
    public:
        virtual int chooseIndex(omnetpp::cMessage *msg) override;
};

#endif