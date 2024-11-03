#include "s2f/messages/SM_ResolverRequest_m.h"
#include "s2f/architecture/dns/common.h"

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