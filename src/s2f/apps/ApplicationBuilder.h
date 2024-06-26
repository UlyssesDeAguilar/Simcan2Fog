#ifndef SIMCAN_EX_APPLICATION_BUILDER
#define SIMCAN_EX_APPLICATION_BUILDER

#include <omnetpp.h>

#include "s2f/management/dataClasses/Applications/Application.h"
#include "s2f/management/dataClasses/Applications/AppParameter.h"

/**
 * @brief This class encapsulates the steps needed for app instantiation
 * @author Ulysses de Aguilar
 */
class ApplicationBuilder
{
public:
    struct Context
    {
        const Application *schema;      // Definition of the app structure (template)
        const std::string *vmId;        // The VM ID (where it will be executed)
        const std::string *appId;       // The Instance ID 
        const std::string *userId;      // The User ID (the user who launched the app)
    };

    /**
     * @brief Builds an app inside the "parent module", which is expected to be an UserAppModule
     * @details This is so it's easier to replace the underlying "real" app module without breaking the apps vector
     * @param parent Pointer to the module
     * @param context The context (information) needed for creating the app
     * @return omnetpp::cModule* A pointer to the application module
     */
    virtual omnetpp::cModule *build(omnetpp::cModule *parent, const Context &context);
    void deleteApp(cModule * parent);

protected:
    omnetpp::cModule *searchAndCreate(cModule *parent, const Context &context);
    virtual void initParameters(cModule *appModule, const Context &context);
    virtual void connectApp(cModule * parent, cModule *app);
    omnetpp::cModule *buildAndStart(omnetpp::cModule *app);
};

#endif /*SIMCAN_EX_APPLICATION_BUILDER*/