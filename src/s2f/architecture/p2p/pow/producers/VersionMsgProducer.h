#ifndef __P2P_VERSIONMSGPRODUCER_H_
#define __P2P_VERSIONMSGPRODUCER_H_

#include "../messages/Version_m.h"
#include "IPowMsgProducer.h"

using namespace s2f::p2p;

class VersionMsgProducer : public IPowMsgProducer
{

  public:
    virtual inet::Packet *buildMessage(struct HandlerContext &ictx) override;
};

#endif
