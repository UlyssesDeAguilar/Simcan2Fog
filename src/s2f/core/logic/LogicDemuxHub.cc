#include "LogicDemuxHub.h"

using namespace omnetpp;
Define_Module(LogicDemuxHub);

void LogicDemuxHub::initialize() 
{
    mainOut = gate("mainOut");
    demuxGates.outBaseId = gate("demuxOut", 0)->getBaseId();
    demuxGates.inBaseId = gate("demuxIn", 0)->getBaseId();
    demux = check_and_cast<IDemux *>(createOne(par("demuxClass")));
}

void LogicDemuxHub::finish()
{
    delete demux;
}

void LogicDemuxHub::handleMessage(omnetpp::cMessage *msg) 
{
    cGate *gate = msg->getArrivalGate();
    if (gate->getBaseId() == demuxGates.inBaseId)
        send(msg, mainOut);
    else
        send(msg, demuxGates.outBaseId + demux->chooseIndex(msg));
}
