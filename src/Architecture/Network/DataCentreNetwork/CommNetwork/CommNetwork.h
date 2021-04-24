#ifndef __SIMCAN_2_0_COMMNETWORK_H_
#define __SIMCAN_2_0_COMMNETWORK_H_

#include "Core/cSIMCAN_Core.h"


class CommNetwork: public cSIMCAN_Core{

    protected:

        cGate** inputGates;
        cGate** outputGates;
        cGate* fromStorageGate;
        cGate* toStorageGate;

        /**
         * Destructor.
         */
         ~CommNetwork();

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
