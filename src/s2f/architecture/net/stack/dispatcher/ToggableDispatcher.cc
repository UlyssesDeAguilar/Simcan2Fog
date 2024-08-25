#include "ToggableDispatcher.h"
#include "s2f/architecture/net/stack/dispatcher/ToggleReq_m.h"

Define_Module(ToggableDispatcher);

void ToggableDispatcher::arrived(cMessage *message, cGate *gate, const SendOptions& options, simtime_t time)
{
    Enter_Method_Silent();

    auto toggleReq = dynamic_cast<ToggleReq *>(message);
    if (toggleReq != nullptr)
    {
        int socketId = toggleReq->getSocketId();
        int gateIndex = toggleReq->getGateIndex();

        auto it = socketIdToGateIndex.find(socketId);

        if (it != socketIdToGateIndex.end())
            socketIdToGateIndex[socketId] = gateIndex;
        else
            throw cRuntimeError("handleMessage(): Socket hasn't been registered: socketId = %d, gate = %d", socketId, gateIndex);

        return;
    }

    MessageDispatcher::arrived(message, gate, options, time);
}
