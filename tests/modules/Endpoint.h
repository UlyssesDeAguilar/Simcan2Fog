#include "Messages/SM_ResolverRequest_m.h"
#include "Architecture/Network/DNS/common.h"
using namespace dns;
using namespace inet;

class Endpoint : public cSimpleModule
{
private:
    const char *nameToResolve;
protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
};