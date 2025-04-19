//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SIMCAN2FOG_SERVERENDPOINT_H_
#define __SIMCAN2FOG_SERVERENDPOINT_H_

#include <omnetpp.h>
#include "s2f/apps/AppBase.h"
#include "s2f/architecture/net/stack/proxy/ProxyServiceRequest_m.h"
#include "s2f/architecture/net/protocol/RestfulRequest_m.h"
#include "s2f/architecture/net/protocol/RestfulResponse_m.h"

using namespace omnetpp;

class ServerEndpoint : public AppBase, public AppBase::ICallback
{
protected:
  int sockFd;
  const char *serviceName;
  virtual void initialize() override;
  virtual void processSelfMessage(cMessage *msg) override;
public:
  virtual void handleDataArrived(int sockFd, Packet *p) override;
  virtual bool handlePeerClosed(int sockFd) override;
  virtual bool handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort) override;
  virtual void handleConnectReturn(int sockFd, bool connected) override {};
  virtual void handleResolutionFinished(const L3Address ip, bool resolved) override {};
};

#endif
