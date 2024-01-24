#include "Applications/Builder/DC/DataCentreApplicationBuilder.h"

using omnetpp::cModule;
using omnetpp::cModuleType;

cModule *DataCentreApplicationBuilder::build(cModule *parent, const ApplicationContext &context)
{
    // Get type from schema
    auto schema = context.schema;
    cModuleType *moduleType = cModuleType::get(schema->getFullPath().c_str());

    // Disconnect and delete dummy app
    deleteApp(parent);

    // Create the app
    cModule *appModule = moduleType->create("app", parent);

    // Init context parameters
    appModule->par("appInstance") = *context.appId;
    appModule->par("vmInstance") = *context.vmId;
    appModule->par("userInstance") = *context.userId;

    // Initialize all the "free" parameters
    for (const auto param : schema->getAllParameters())
    {
        auto moduleParam = &appModule->par(param->getName().c_str());
        param->initModuleParameter(moduleParam);
    }

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

    appModule->finalizeParameters();

    // Connect gates
    parent->gate("fromHub")->connectTo(appModule->gate("in"));
    appModule->gate("out")->connectTo(parent->gate("toHub"));

    // Create internals and schedule start
    appModule->buildInside();
    appModule->scheduleStart(simTime());

    // Initialize the module
    appModule->callInitialize();
    return appModule;
}

void DataCentreApplicationBuilder::deleteApp(cModule *appVector)
{
    // Retrieve module -> call finish
    auto oldApp = appVector->getSubmodule("app");
    oldApp->callFinish();

    // Disconnect
    appVector->gate("fromHub")->disconnect();
    appVector->gate("toHub")->getPreviousGate()->disconnect();

    // Release and delete
    oldApp->deleteModule();
}
