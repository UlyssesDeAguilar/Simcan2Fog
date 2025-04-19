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
    const Application* schema = context.schema;
    std::string fullPath = schema->getPath() + "." + schema->getType();
    cModuleType *moduleType = cModuleType::get(fullPath.c_str());

    // Reserve the necessary space (if needed)
    if ((bool)parent->par("defaultInitialized"))
    {
        deleteApp(parent);
        parent->par("defaultInitialized").setBoolValue(false);
    }
    // Create and return app
    return moduleType->create("app", parent);
}

void ApplicationBuilder::initParameters(cModule *appModule, const Context &context)
{
    // Init context parameters
    appModule->par("appInstance") = context.appId;
    appModule->par("vmInstance") = context.vmId;
    appModule->par("userInstance") = context.userId;

    // Initialize all the "free" parameters
    for (const auto &iter : context.schema->getAllParameters())
    {
        cPar &param = appModule->par(iter.first.c_str());
        if (param.getType() == cPar::STRING)
            param.setStringValue(iter.second.c_str());
        else
            param.parse(iter.second.c_str());
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
    parent->gate("socketIn")->connectTo(app->gate("socketIn"));
    app->gate("socketOut")->connectTo(parent->gate("socketOut"));
}

void ApplicationBuilder::deleteApp(cModule *parent)
{
    // Retrieve module -> call finish
    auto oldApp = parent->getSubmodule("app");
    oldApp->callFinish();

    // Disconnect
    parent->gate("fromHub")->disconnect();
    parent->gate("toHub")->getPreviousGate()->disconnect();
    parent->gate("socketIn")->disconnect();
    parent->gate("socketOut")->getPreviousGate()->disconnect();

    // Release and delete
    oldApp->deleteModule();
}
