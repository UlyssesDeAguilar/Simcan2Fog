#ifndef SIMCAN_EX_LOGIC_DEMUX_HUB__
#define SIMCAN_EX_LOGIC_DEMUX_HUB__

#include <omnetpp.h>
#include "s2f/core/include/SIMCAN_types.h"

class LogicDemuxHub : public omnetpp::cSimpleModule
{
    public:
        class IDemux : public omnetpp::cObject
        {
            public:
                virtual int chooseIndex(omnetpp::cMessage *msg) = 0;
        };

    protected:
        omnetpp::cGate *mainOut{};
        GateInfo demuxGates;
        IDemux *demux{};
        virtual void initialize() override;
        virtual void finish() override;
        virtual void handleMessage(omnetpp::cMessage *msg) override;
};

#endif
