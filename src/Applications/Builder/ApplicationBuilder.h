#ifndef SIMCAN_EX_APPLICATION_BUILDER
#define SIMCAN_EX_APPLICATION_BUILDER

#include <omnetpp.h>
#include "Messages/SM_UserAPP.h"
#include "Management/dataClasses/Applications/Application.h"
#include "Management/dataClasses/Applications/AppParameter.h"
#include "Applications/UserApps/LocalApplication/LocalApplication.h"

class ApplicationBuilder
{

public:
    struct ApplicationContext
    {
        const Application *schema;
        const std::string *vmId;
        const std::string *appId;
        const std::string *userId;
    };

    virtual omnetpp::cModule *build(omnetpp::cModule *parent, const ApplicationContext &context) = 0;
};
#endif /*SIMCAN_EX_APPLICATION_BUILDER*/