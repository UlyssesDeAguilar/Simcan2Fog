#include "s2f/architecture/net/stack/proxy/Session.h"

using namespace omnetpp;
using namespace inet;

Session::~Session()
{
    if (pendingQueue != nullptr)
        delete pendingQueue;
    if (socketInd != nullptr)
        delete socketInd;
}

void Session::pushToPendingQueue(cMessage *msg)
{
    if (pendingQueue == nullptr)
        pendingQueue = new cQueue("pendingQueue");
    pendingQueue->insert(msg);
}

Message *Session::setSessionPending(int gateIndex)
{
    state = PENDING;
    Message *ptr = this->socketInd;
    this->socketInd = nullptr;
    this->gateIndex = gateIndex;
    return ptr;
}

cQueue *Session::setSessionEstablished()
{
    state = ESTABLISHED;
    cQueue *ptr = this->pendingQueue;
    this->pendingQueue = nullptr;
    return ptr;
}