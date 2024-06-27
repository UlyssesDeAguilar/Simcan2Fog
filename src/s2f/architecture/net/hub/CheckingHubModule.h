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

#ifndef __SIMCAN_2_0_CHEKING_HUB_MODULE_H_
#define __SIMCAN_2_0_CHEKING_HUB_MODULE_H_

#include "s2f/core/cSIMCAN_Core.h"

class CheckingHubModule : public cSIMCAN_Core
{
protected:
    int upperSize;
    int lowerSize;
    GateInfo lower;
    GateInfo upper;
    bool staticAppAssignment;

    virtual void initialize() override;
    virtual void finish() override;

private:
    
    virtual cGate *getOutGate(cMessage *msg) override;
    virtual void processSelfMessage(cMessage *msg) override;
    virtual void processRequestMessage(SIMCAN_Message *sm) override;
    virtual void processResponseMessage(SIMCAN_Message *sm) override;
};

#endif
