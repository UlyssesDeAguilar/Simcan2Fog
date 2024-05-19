#include "ApplicationBuilder.h"

using omnetpp::cModule;
using omnetpp::cModuleType;

cModule *ApplicationBuilder::build(cModule *parent, const Context &context)
{
    // Create the app
    cModule *app = searchAndCreate(parent, context);

    // Init the app parameters
    initParameters(app, context);

    // Connect the app
    connectApp(parent, app);

    // Finalize everything and start
    return buildAndStart(app);
}

cModule *ApplicationBuilder::searchAndCreate(cModule *parent, const Context &context)
{
    // Get type from schema
    auto schema = context.schema;
    cModuleType *moduleType = cModuleType::get(schema->getFullPath().c_str());

    // Reserve the necessary space (if needed)
    if ((bool) parent->par("defaultInitialized") == true)
        deleteApp(parent);

    // Create and return app
    return moduleType->create("app", parent);
}

void ApplicationBuilder::initParameters(cModule *appModule, const Context &context)
{
    // Init context parameters
    appModule->par("appInstance") = *context.appId;
    appModule->par("vmInstance") = *context.vmId;
    appModule->par("userInstance") = *context.userId;

    // Initialize all the "free" parameters
    for (const auto param : context.schema->getAllParameters())
    {
        auto moduleParam = &appModule->par(param->getName().c_str());
        param->initModuleParameter(moduleParam);
    }

    // Finalize the parameters
    appModule->finalizeParameters();
}

cModule *ApplicationBuilder::buildAndStart(cModule *appModule)
{
    // Create internals and schedule start
    appModule->buildInside();
    appModule->scheduleStart(simTime());

    // Initialize the module
    appModule->callInitialize();

    return appModule;
}

void ApplicationBuilder::connectApp(cModule *parent, cModule *app)
{
    // Connect gates
    parent->gate("fromHub")->connectTo(app->gate("in"));
    app->gate("out")->connectTo(parent->gate("toHub"));
}

void ApplicationBuilder::deleteApp(cModule *parent)
{
    // Retrieve module -> call finish
    auto oldApp = parent->getSubmodule("app");
    oldApp->callFinish();

    // Disconnect
    parent->gate("fromHub")->disconnect();
    parent->gate("toHub")->getPreviousGate()->disconnect();

    // Release and delete
    oldApp->deleteModule();
}