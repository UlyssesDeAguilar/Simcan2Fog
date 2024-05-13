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

#ifndef SIMCAN_EX_PULLSERVICE_H_
#define SIMCAN_EX_PULLSERVICE_H_

#include "Architecture/Network/MessageQueue/MessageQueueManager.h"
#include "Architecture/Network/MessageQueue/Internals/SinkDelegate.h"
#include "Messages/INET_AppMessage.h"
#include "Messages/SIMCAN_Message.h"

#include <omnetpp.h>

using namespace omnetpp;

class PullService : public cSimpleModule
{
protected:
  MessageQueueManager *manager{}; //!< Reference of the Queue Manager
  SinkDelegate *delegate{};       //!< Reference to the Sink Delegate
  bool topicEstablished = false;  //!< If the topic was established
  int directInId;                 //!< Id for the communication with the delegate
  virtual void initialize();
  virtual void handleMessage(cMessage *msg);
};

#endif
