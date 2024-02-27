#include "Switch.h"

Define_Module(Switch);

void Switch::initialize()
{
    // Init the super-class
    cSIMCAN_Core::initialize();

    // Get gates information
    upper.inBaseId = gateBaseId("upper$i");
    upper.outBaseId = gateBaseId("upper$o");
    lower.inBaseId = gateBaseId("comm$i");
    lower.outBaseId = gateBaseId("comm$o");

    // Get level from the parent module
    // cModule *parent = getParentModule();
    // const char *l = parent->getProperties()->get("smlevel")->getValue();
    // level = std::stoi(l);
    // localIdx = parent->getIndex();
}

void Switch::finish()
{
    // Finish the super-class
    cSIMCAN_Core::finish();
}

cGate *Switch::getOutGate(cMessage *msg)
{
    cGate *arrivalGate = msg->getArrivalGate();
    int gateId = arrivalGate->getBaseId();

    // Request came from upper
    if (gateId == upper.inBaseId)
        return gate(upper.outBaseId);

    // Request came from lower
    if (gateId == lower.inBaseId)
        return gate(lower.outBaseId + arrivalGate->getIndex());

    error("Gate not found");
    return nullptr;
}

void Switch::processRequestMessage(SIMCAN_Message *sm)
{
    auto routingInfo = check_and_cast<RoutingInfo *>(sm->getControlInfo());
    auto requestUrl = routingInfo->getUrl();

    // If it hasn't a local ip set up
    if (~(requestUrl.getState()) & ServiceURL::LOCAL)
        error("Message doesn't contain at least the LocalIp");

    // Retrieve destination address
    uint32_t address = requestUrl.getLocalAddress().getInt();
    if (address == DC_MANAGER_LOCAL_ADDR)
        sendRequestMessage(sm, upper.outBaseId);
    else
        sendRequestMessage(sm, lower.outBaseId + address);
}

void Switch::processResponseMessage(SIMCAN_Message *sm)
{
    // Debug (Debug)
    if (getEnvir()->isLoggingEnabled() && debugSimcanCore)
    {
        EV_DEBUG << "(processResponseMessage) Sending response message." << '\n'
                 << sm->contentsToString(showMessageContents, showMessageTrace) << '\n';
    }

    // Relay the message
    sendResponseMessage(sm);
}
