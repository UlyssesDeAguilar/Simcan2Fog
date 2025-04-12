#include "s2f/core/include/signals.h"

using namespace omnetpp;

simsignal_t maxCores = cComponent::registerSignal("maxCores");
simsignal_t maxRam = cComponent::registerSignal("maxRam");
simsignal_t maxDisk = cComponent::registerSignal("maxDisk");
simsignal_t maxVms = cComponent::registerSignal("maxVms");
simsignal_t allocatedCores = cComponent::registerSignal("allocatedCores");
simsignal_t allocatedRam = cComponent::registerSignal("allocatedRam");
simsignal_t allocatedDisk = cComponent::registerSignal("allocatedDisk");
simsignal_t allocatedVms = cComponent::registerSignal("allocatedVms");