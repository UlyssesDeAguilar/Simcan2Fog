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

#ifndef SIMCAN_EX_PUSHSERVICE_H_
#define SIMCAN_EX_PUSHSERVICE_H_

#include <omnetpp.h>

#include "s2f/messages/INET_AppMessage.h"
#include "s2f/messages/SIMCAN_Message.h"
#include "s2f/architecture/messagequeue/MessageQueueManager.h"

using namespace omnetpp;

class PushService : public cSimpleModule
{
protected:
  MessageQueueManager * manager{};
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
};

#endif
