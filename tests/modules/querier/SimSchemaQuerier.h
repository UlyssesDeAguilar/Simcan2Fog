#include <omnetpp.h>
#include "Core/SimSchema/SimSchema.h"

class SimSchemaQuerier : public cSimpleModule
{
protected:
    simschema::SimSchema* simSchema;
    
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

    void testApp(const std::string& appName);
    void testVM(const std::string& vmType);
    void testSLA(const std::string& name);
    void testVMCost(const std::string &sla, const std::string &vmType);
};