#include "s2f/core/include/SIMCAN_types.h"
#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/messages/SIMCAN_Message.h"

namespace s2f
{
    namespace tests
    {
        class MQDriver : public cSIMCAN_Core
        {
        protected:
            const char *destinationTopic;
            virtual void initialize() override;
            virtual void finish() override {};
            virtual cGate *getOutGate(cMessage *sm) override;
            virtual void processRequestMessage(SIMCAN_Message *sm) override;
            virtual void processResponseMessage(SIMCAN_Message *sm) override;
            virtual void processSelfMessage(cMessage *msg) override;
        };

    }
}
