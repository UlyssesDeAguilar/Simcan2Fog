#include "Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.h"
#include "Applications/Builder/DC/DataCentreApplicationBuilder.h"
#include "Applications/UserApps/LocalApplication/LocalApplication.h"


using omnetpp::cModule;
using omnetpp::cModuleType;

void DataCentreApplicationBuilder::initParameters(cModule *appModule, const Context &context)
{
    // SEE THIS -- Seems like an update check or relaunch from the DC
    unsigned int *appStateArr = dataCenter->getAppModuleById(*context.appId);
    if (appStateArr != nullptr)
    {
        auto instance = dynamic_cast<LocalApplication *>(appModule);
        instance->setCurrentIteration(appStateArr[0]);
        instance->setCurrentRemainingMIs(appStateArr[1]);
        dataCenter->mapAppsModulePerId.erase(*context.appId);
        appModule->par("initialized") = true;
    }

    // Finish the parameters
    ApplicationBuilder::initParameters(appModule, context);
}
