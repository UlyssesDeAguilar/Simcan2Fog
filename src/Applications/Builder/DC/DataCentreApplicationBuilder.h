#ifndef SIMCAN_EX_DC_APPLICATION_BUILDER
#define SIMCAN_EX_DC_APPLICATION_BUILDER

#include "Applications/Builder/ApplicationBuilder.h"
// Forward declaration
class DataCentreManagerBase;

class DataCentreApplicationBuilder : public ApplicationBuilder
{
private:
    DataCentreManagerBase *dataCenter;

protected:
    virtual void initParameters(cModule *appModule, const Context &context) override;

public:
    void setManager(DataCentreManagerBase *dc) { this->dataCenter = dc; }
    void deleteApp(omnetpp::cModule *appModule) { ApplicationBuilder::deleteApp(appModule); }
};

#endif /*SIMCAN_EX_APPLICATION_BUILDER*/
