#include <omnetpp.h>
#include "s2f/core/simdata/DataManager.h"

class DataManagerQuerier : public cSimpleModule
{
protected:
    DataManager* dm;
    
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

    void testApp(const std::string& appName);
    void testVM(const std::string& vmType);
    void testSLA(const std::string& name);
};