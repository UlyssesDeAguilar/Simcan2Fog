#ifndef __SIMCAN_EX_TOGGABLE_DISPATCHER_H
#define __SIMCAN_EX_TOGGABLE_DISPATCHER_H

#include "inet/common/MessageDispatcher.h"
using namespace inet;

class ToggableDispatcher : public inet::MessageDispatcher
{
protected:
    virtual void arrived(cMessage *message, cGate *gate, const SendOptions& options, simtime_t time) override;
};

#endif
