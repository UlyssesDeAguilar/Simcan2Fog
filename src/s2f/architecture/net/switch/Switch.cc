#include "../../net/switch/Switch.h"

Define_Module(Switch);

void Switch::initialize()
{
    // Get gates information
    manager.inBaseId = gateBaseId("manager$i");
    manager.outBaseId = gateBaseId("manager$o");
    network.inBaseId = gateBaseId("netStack$i");
    network.outBaseId = gateBaseId("netStack$o");
    lower.inBaseId = gateBaseId("comm$i");
    lower.outBaseId = gateBaseId("comm$o");
}

void Switch::handleMessage(cMessage *msg)
{
    auto routingInfo = check_and_cast<RoutingInfo *>(msg->getControlInfo());
    auto requestUrl = routingInfo->getDestinationUrl();

    // If it hasn't a local ip set up
    if (~(requestUrl.getState()) & ServiceURL::LOCAL)
        error("Message doesn't contain at least the LocalIp");

    // Log details of routing!
    EV << "(Switch) Sending message of type: " << msg->getClassName()
       << " to: " << routingInfo->getDestinationUrl()
       << " from: " << routingInfo->getSourceUrl() << "\n";

    // Retrieve destination address
    uint32_t address = requestUrl.getLocalAddress().getInt();
    
    if (address == DC_MANAGER_LOCAL_ADDR)
        send(msg, manager.outBaseId);
    else if( address == DC_NETWORK_STACK)
        send(msg, network.outBaseId);
    else
        send(msg, lower.outBaseId + address);
}
