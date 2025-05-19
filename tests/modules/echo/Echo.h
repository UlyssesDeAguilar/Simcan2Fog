#include "s2f/core/include/SIMCAN_types.h"
#include "s2f/core/cSIMCAN_Core.h"

namespace s2f
{
    namespace tests
    {
        using namespace inet;
        class Echo : public cSimpleModule
        {
        private:
            bool recv;        //!< Recieve or Send mode
            int ip;           //!< Ip of the node
            int targetIp;     //!< Initial target IP address
            int addressRange; //!< Address range to test (sequential range based on base IP)
            int numAcks;

        protected:
            virtual void initialize() override;
            virtual void finish() override;
            virtual void handleMessage(cMessage *msg) override;
        };
    }
}