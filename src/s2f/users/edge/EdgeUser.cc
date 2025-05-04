#include "EdgeUser.h"
#include "s2f/messages/SM_AppSettings_m.h"

using namespace users;
Define_Module(EdgeUser);

void EdgeUser::initialize() {
    // Build the userId
    opp_string_map dependencies;
    userId = generateUniqueId(getFullName()).c_str();
    deploymentZone = par("deploymentZone");

    auto vmsArray = check_and_cast<cValueArray*>(par("vms").objectValue());
    for (int i = 0, size = vmsArray->size(); i < size; i++) {
        auto vmDefinition = check_and_cast<cValueMap*>(
                vmsArray->get(i).objectValue());
        const char *name = vmDefinition->get("name");
        const char *type = vmDefinition->get("type");

        auto &vm = managedVms[generateUniqueId(name).c_str()];
        vm.setName(name);
        vm.setType(type);
        EV_DEBUG << "Loaded VM " << vm.getName() << " of type " << vm.getType()
                        << "\n";
    }

    auto localAppsArray = check_and_cast<cValueArray*>(
            par("localApps").objectValue());
    for (int i = 0, size = localAppsArray->size(); i < size; i++) {
        auto appDefinition = check_and_cast<cValueMap*>(
                localAppsArray->get(i).objectValue());
        const char *name = appDefinition->get("name");
        const char *type = appDefinition->get("type");

        localApps.emplace_back();
        auto &app = localApps.back();
        app.setName(name);
        app.setType(type);

        if (appDefinition->containsKey("dependsOn"))
            dependencies[(const char*) appDefinition->get("dependsOn")] = name;

        EV_DEBUG << "Loaded Local App " << app.getName() << " of type "
                        << app.getType() << "\n";
    }

    auto remoteAppsArray = check_and_cast<cValueArray*>(
            par("remoteApps").objectValue());
    for (int i = 0, size = remoteAppsArray->size(); i < size; i++) {
        auto appDefinition = check_and_cast<cValueMap*>(
                remoteAppsArray->get(i).objectValue());
        const char *name = appDefinition->get("name");
        const char *type = appDefinition->get("type");
        const char *platform = appDefinition->get("platform");

        App app;
        auto iter = dependencies.find(name);
        if (iter != dependencies.end())
            app.setDependant(iter->second.c_str());

        app.setName(
                par("makeServiceNameUnique") ?
                        generateUniqueId(name).c_str() : name);
        app.setType(type);
        findPlatform(platform)->appendApp(app);

        EV_DEBUG << "Loaded App " << app.getName() << " of type "
                        << app.getType() << " to be deployed on VM " << platform
                        << "\n";
    }

    cModule *area = getParentModule();
    auto sensorsArray = check_and_cast<cValueArray*>(par("sensors").objectValue());
    managedSensors.reserve(sensorsArray->size());

    cPatternMatcher matcher;
    for (int i = 0, size = sensorsArray->size(); i < size; i++) {
        const char *sensorPattern = sensorsArray->get(i).stringValue();
        matcher.setPattern(sensorPattern, true, true, true);

        for (cModule::SubmoduleIterator it(area); !it.end(); it++) {
            cModule *submodule = *it;
            const char *submoduleName = submodule->getFullName();

            if (matcher.matches(submoduleName)) {
                EV << "Found sensor: " << submoduleName << " matching expression or name: " << sensorPattern << "\n";
                managedSensors.push_back(submodule);
            }
        }
    }

    serialGates.inBaseId = gateBaseId("serialIn");
    serialGates.outBaseId = gateBaseId("serialOut");

    auto startMsg = new cMessage("User start");
    scheduleAt(par("startupTime"), startMsg);

    // WATCH_MAP(managedVms);
    // WATCH_MAP(remoteApps);
    // WATCH_VECTOR(localApps);
    WATCH(userId);
}

void EdgeUser::finish() {
    managedVms.clear();
    localApps.clear();
    managedSensors.clear();
}

void EdgeUser::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        startup();
        delete msg;
        return;
    }

    cGate *gate = msg->getArrivalGate();
    if (gate->getBaseId() == serialGates.inBaseId) {
        processSerialEvent(msg);
        return;
    }

    SIMCAN_Message *sm = check_and_cast<SIMCAN_Message*>(msg);
    if (sm->getOperation() == SM_VM_Req)
        handleVmUpdate(check_and_cast<SM_UserVM*>(msg));
    else if (sm->getOperation() == SM_APP_Req)
        handleAppEvent(check_and_cast<SM_UserAPP*>(msg));
    else
        error("Unexpected message kind with code %d", sm->getOperation());
}

void EdgeUser::startup() {
    EV_DEBUG << "Starting deployment of vms. #vms: " << managedVms.size()
                    << "\n";

    for (auto &vmEntry : managedVms) {
        Vm &vm = vmEntry.second;

        // TODO: Accept this as a customizable parameter
        VM_Request::InstanceRequestTimes times;
        times.maxStartTime = 1000;
        times.rentTime = 3600;
        times.maxSubTime = 1000;
        times.maxSubscriptionTime = 1000;

        auto request = new SM_UserVM();
        request->setOperation(SM_VM_Req);
        request->setUserId(userId.c_str());
        request->setZone(deploymentZone);
        request->createNewVmRequest(vm.getType(), vmEntry.first.c_str(), times);
        request->setDestinationTopic("cloudProvider");

        // Compatibility with SIMCAN modules
        request->addNodeToTrace(getFullPath());
        request->addModuleToTrace(getId(), gate("queueOut")->getId(), 0);
        send(request, "queueOut");
    }

    if (localApps.size() > 0) {
        // Deploy local applications
        EV_DEBUG << "Starting deployment of local apps. #apps "
                        << localApps.size() << "\n";
        auto appRequest = new SM_UserAPP();
        appRequest->setOperation(SM_APP_Req);
        appRequest->setUserId(userId.c_str());
        appRequest->setVmId(par("controllerHostName"));

        for (auto &appEntry : localApps)
            appRequest->createNewAppRequest(appEntry.getName(),
                    appEntry.getType(), 0.0);

        send(appRequest, serialGates.outBaseId);
    }

    // Also turn the sensors on
    if (managedSensors.size() > 0)
        turnSensorsOn();
}

void EdgeUser::processSerialEvent(cMessage *msg) {
    // TODO: It would be a bit weird for the moment
    EV_WARN << "Ignoring serial event" << "\n";
    delete msg;
}

void EdgeUser::handleVmUpdate(SM_UserVM *update) {
    for (int i = 0, size = update->getVmArraySize(); i < size; i++) {
        const char *vmId = update->getVm(i).vmId.c_str();
        const VM_Response *response = update->getResponse(i);
        Vm &vmDefinition = managedVms[vmId];

        if (response) {
            EV_DEBUG << "Vm is ready: " << vmDefinition.getName() << "\n";
            vmDefinition.setState(State::UP);

            const char *topic = update->getReturnTopic();
            const char *hypervisorUrl = response->ip.c_str();
            auto request = new SM_UserAPP();
            request->setVmId(vmId);
            request->setDestinationTopic(topic);
            request->setHypervisorUrl(hypervisorUrl);
            request->setUserId(userId.c_str());
            request->setHypervisorUrl(response->ip.c_str());

            for (int i = 0, size = vmDefinition.getAppArraySize(); i < size;
                    i++) {
                auto &app = vmDefinition.getAppForUpdate(i);
                request->createNewAppRequest(app.getName(), app.getType(),
                        simTime().dbl());
                app.setState(State::UP);

                if (app.getDependant())
                    sendServiceIsUp(app.getDependant(), app,
                            update->getDomain());
            }

            // Compatibility with SIMCAN modules
            request->addNodeToTrace(getFullPath());
            request->addModuleToTrace(getId(), gate("queueOut")->getId(), 0);
            send(request, "queueOut");
        } else {
            EV_WARN << "Will not proceed with vm" << vmDefinition.getName()
                           << "\n";
        }
    }
    delete update;
}

void EdgeUser::handleAppEvent(SM_UserAPP *update) {
    // TODO: It would be a bit weird for the moment
    EV_WARN << "Ignoring app event" << "\n";
    delete update;
}

void EdgeUser::turnSensorsOn() {
    EV_DEBUG << "Turning all sensors on\n";
    for (auto sensor : managedSensors)
        sensor->par("powerOn") = true;
}

void EdgeUser::turnSensorsOff() {
    EV_DEBUG << "Turning all sensors off\n";
    for (auto sensor : managedSensors)
        sensor->par("powerOn") = false;
}

opp_string EdgeUser::generateUniqueId(const char *name) {
    return std::string(name) + std::string("::") + std::to_string(getId());
}

users::Vm* EdgeUser::findPlatform(const char *name) {
    for (auto &vm : managedVms) {
        Vm &def = vm.second;
        if (opp_strcmp(def.getName(), name) == 0)
            return &def;
    }

    error("Could not find platform %s", name);
    return nullptr;
}

void EdgeUser::sendServiceIsUp(const char *localApp, const App &service,
        const char *domain) {
    opp_string route = service.getName();
    route = route + opp_string(".") + domain;

    AppParameter param;
    param.name = "serviceName";
    param.value = route;

    auto request = new SM_AppSettings();
    request->setAppName(localApp);
    request->setVmId(par("controllerHostName"));
    request->appendParameters(param);
    send(request, serialGates.outBaseId);
}
