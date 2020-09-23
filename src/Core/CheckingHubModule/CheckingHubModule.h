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

#include "Core/cSIMCAN_Core.h"


class CheckingHubModule: public cSIMCAN_Core{

    protected:

        cGate** fromInputGates;
        cGate** fromOutputGates;
        cGate** toInputGates;
        cGate** toOutputGates;
        bool staticAppAssignment;

        /**
         * Destructor.
         */
         ~CheckingHubModule();

        /**
         *  Module initialization.
         */
         void initialize();

        /**
         * Module ending.
         */
         void finish();


    private:

        /**
         * Get the outGate ID to the module that sent <b>msg</b>
         *
         * @param msg Arrived message.
         * @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
         */
         cGate* getOutGate (cMessage *msg);

        /**
         * Process a self message.
         *
         * @param msg Self message.
         */
         void processSelfMessage (cMessage *msg);

        /**
         * Process a request message.
         *
         * @param sm Request message.
         */
         void processRequestMessage (SIMCAN_Message *sm);

        /**
         * Process a response message.
         *
         * @param sm Request message.
         */
         void processResponseMessage (SIMCAN_Message *sm);
};

#endif
