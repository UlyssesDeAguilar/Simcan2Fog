#include "EdgeUser.h"

using namespace users;
Define_Module(EdgeUser);

void EdgeUser::initialize()
{
    // Build the userId
    userId = generateUniqueId(getFullName()).c_str();
    deploymentZone = par("deploymentZone");

    auto vmsArray = check_and_cast<cValueArray *>(par("vms").objectValue());
    for (int i = 0, size = vmsArray->size(); i < size; i++)
    {
        auto vmDefinition = check_and_cast<cValueMap *>(vmsArray->get(i).objectValue());
        const char *name = vmDefinition->get("name");
        const char *type = vmDefinition->get("type");

        auto &vm = managedVms[generateUniqueId(name).c_str()];
        vm.setName(name);
        vm.setType(type);
        EV_DEBUG << "Loaded VM " << vm.getName() << " of type " << vm.getType() << "\n";
    }

    auto remoteAppsArray = check_and_cast<cValueArray *>(par("remoteApps").objectValue());

    for (int i = 0, size = remoteAppsArray->size(); i < size; i++)
    {
        auto appDefinition = check_and_cast<cValueMap *>(remoteAppsArray->get(i).objectValue());
        const char *name = appDefinition->get("name");
        const char *type = appDefinition->get("type");
        const char *platform = appDefinition->get("platform");

        auto &app = remoteApps[generateUniqueId(name).c_str()];
        app.setName(name);
        app.setType(type);
        app.setPlatform(findPlatform(platform));
        EV_DEBUG << "Loaded App " << app.getName() << " of type " << app.getType() << " to be deployed on VM " << app.getPlatform()->getName() << "\n";
    }

    auto localAppsArray = check_and_cast<cValueArray *>(par("localApps").objectValue());
    for (int i = 0, size = localAppsArray->size(); i < size; i++)
    {
        auto appDefinition = check_and_cast<cValueMap *>(localAppsArray->get(i).objectValue());
        const char *name = appDefinition->get("name");
        const char *type = appDefinition->get("type");

        localApps.emplace_back();
        auto &app = localApps.back();
        app.setName(name);
        app.setType(type);
        EV_DEBUG << "Loaded Local App " << app.getName() << " of type " << app.getType() << "\n";
    }

    cModule *zone = getParentModule();
    auto sensorsArray = check_and_cast<cValueArray *>(par("sensors").objectValue());
    managedSensors.reserve(sensorsArray->size());

    for (int i = 0, size = sensorsArray->size(); i < size; i++)
    {
        const char *sensorName = sensorsArray->get(i).stringValue();
        cModule *mod = zone->getSubmodule(sensorName);
        if (!mod)
            error("Sensor %s not found", sensorName);
        managedSensors.push_back(mod);
    }

    serialGates.inBaseId = gateBaseId("serialIn");
    serialGates.outBaseId = gateBaseId("serialOut");

    auto startMsg = new cMessage("User start");
    scheduleAt(par("startupTime"), startMsg);

    //WATCH_MAP(managedVms);
    //WATCH_MAP(remoteApps);
    //WATCH_VECTOR(localApps);
    WATCH(userId);
}

void EdgeUser::finish()
{
    managedVms.clear();
    remoteApps.clear();
    localApps.clear();
    managedSensors.clear();
}

void EdgeUser::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        startup();
        delete msg;
        return;
    }

    cGate *gate = msg->getArrivalGate();
    if (gate->getBaseId() == serialGates.inBaseId)
    {
        processSerialEvent(msg);
        return;
    }

    SIMCAN_Message *sm = check_and_cast<SIMCAN_Message *>(msg);
    if (sm->getOperation() == SM_VM_Req)
        handleVmUpdate(check_and_cast<SM_UserVM *>(msg));
    else if (sm->getOperation() == SM_APP_Req)
        handleAppEvent(check_and_cast<SM_UserAPP *>(msg));
    else
        error("Unexpected message kind with code %d", sm->getOperation());
}

void EdgeUser::startup()
{
	EV_DEBUG << "Starting deployment of vms. #vms: " << managedVms.size() << "\n";
    for (auto &vmEntry : managedVms)
    {
        auto request = new SM_UserVM();
        request->setOperation(SM_VM_Req);
        request->setUserId(userId.c_str());
        request->setZone(deploymentZone);
        // TODO: Put in place the rest of the details -> name, type, sla, etc ...
        request->setDestinationTopic("cloudProvider");
        send(request, "queueOut");
    }

    EV_DEBUG << "Starting deployment of local apps. #apps " << localApps.size() << "\n";
    // Deploy local applications
    auto appRequest = new SM_UserAPP();
    appRequest->setOperation(SM_APP_Req);
    appRequest->setUserId(userId.c_str());
    appRequest->setVmId(par("controllerHostName"));

    for (auto &appEntry : localApps)
        appRequest->createNewAppRequest(appEntry.getName(), appEntry.getType(), 0.0);
    
    send(appRequest, serialGates.outBaseId);

    // Also turn the sensors on
    turnSensorsOn();
}

void EdgeUser::processSerialEvent(cMessage *msg)
{
    // TODO: It would be a bit weird for the moment
    EV_WARN << "Ignoring serial event" << "\n";
    delete msg;
}

void EdgeUser::handleVmUpdate(SM_UserVM *update)
{
    for (int i = 0, size = update->getVmArraySize(); i < size; i++)
    {
        VM_Response *response;
        auto &vmDefinition = managedVms[update->getVm(i).vmId];
        if (update->getResponse(i, &response))
        {
            // Should probably redesing the VM request message
            // Right now what's needed is (1) return topic and (2) HypervisorURL
            // And adding the applications (of course)
        }
        else
        {
            EV_WARN << "Will not proceed with vm" << vmDefinition.getName() << "\n";
        }
    }
    delete update;
}

void EdgeUser::handleAppEvent(SM_UserAPP *update)
{
    // TODO: It would be a bit weird for the moment
    EV_WARN << "Ignoring app event" << "\n";
    delete update;
}

void EdgeUser::turnSensorsOn()
{
    EV_DEBUG << "Turning all sensors on\n";
    for (auto sensor : managedSensors)
        sensor->par("powerOn") = true;
}

void EdgeUser::turnSensorsOff()
{
    EV_DEBUG << "Turning all sensors off\n";
    for (auto sensor : managedSensors)
        sensor->par("powerOn") = false;
}

opp_string EdgeUser::generateUniqueId(const char *name)
{
    return std::string(name) + std::string("::") + std::to_string(getId());
}

users::Vm *EdgeUser::findPlatform(const char *name)
{
    for (auto &vm : managedVms)
    {
        Vm &def = vm.second;
        if (opp_strcmp(def.getName(), name) == 0)
            return &def;
    }

    error("Could not find platform %s", name);
    return nullptr;
}
