#ifndef SIMCAN_EX_DC_APPLICATION_BUILDER
#define SIMCAN_EX_DC_APPLICATION_BUILDER

#include "Applications/Builder/ApplicationBuilder.h"

// Forward declaration
class DataCentreManagerBase;

class DataCentreApplicationBuilder : public ApplicationBuilder
{
private:
    DataCentreManagerBase *dataCenter;

public:
    void setManager(DataCentreManagerBase *dc) { this->dataCenter = dc; }
    void deleteApp(omnetpp::cModule *appVector);
    virtual omnetpp::cModule *build(omnetpp::cModule *parent, const ApplicationContext &context) override;
};

#include "Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"

#endif /*SIMCAN_EX_APPLICATION_BUILDER*/
