#include "HardcodedDiscoveryService.h"
#include "inet/common/InitStages.h"
#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/checkandcast.h"
#include "s2f/apps/p2p/InnerRequest_m.h"
#include "s2f/apps/p2p/discovery/DiscoveryResolution_m.h"

Define_Module(HardcodedDiscoveryService);

void HardcodedDiscoveryService::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        AppBase::initialize();
        addressList.clear();
        setReturnCallback(this);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER)
    {
        auto addresses = check_and_cast<cValueArray *>(par("addressList").objectValue())->asStringVector();
        L3AddressResolver resolver;

        for (auto &a : addresses)
        {
            L3Address addr = resolver.resolve(a.c_str());
            addressList.push_back(addr);
        }
    }
}

void HardcodedDiscoveryService::handleMessage(omnetpp::cMessage *msg)
{
    auto arrivalGate = msg->getArrivalGate();
    if (!arrivalGate || strncmp(arrivalGate->getName(), "internal", 9) != 0)
    {
        AppBase::handleMessage(msg);
        return;
    }

    // Handle a discovery request
    auto request = static_cast<InnerRequest *>(msg);
    caller = getSimulation()->getModule(request->getModuleId());

    const auto &response = new DiscoveryResolution("Hardcoded");

    for (const auto &ip : addressList)
        response->appendResolution(ip);

    sendDirect(response, caller, "discovery");
    delete msg;
}
