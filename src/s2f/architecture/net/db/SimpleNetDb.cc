#include "s2f/architecture/net/db/SimpleNetDb.h"

using namespace omnetpp;
Define_Module(SimpleNetDb);

void SimpleNetDb::initialize() {}

void SimpleNetDb::finish()
{
    addressToHostnameMap.clear();
    hostnameToAddressMap.clear();
}

void SimpleNetDb::handleMessage(cMessage *msg)
{
    delete msg;
    error("This module doesn't take any messages");
}

void SimpleNetDb::registerHost(const char *hostName, int address)
{
    Enter_Method_Silent("registerHost: %s :: %d", hostName, address);
    hostnameToAddressMap[hostName] = address;
    addressToHostnameMap[address] = hostName;
}

int SimpleNetDb::getAddress(const char *hostName)
{
    Enter_Method_Silent("queryAddress: %s", hostName);
    return hostnameToAddressMap.at(hostName);
}

const char *SimpleNetDb::getHostName(int address)
{
    Enter_Method_Silent("queryHostName: %d", address);
    return addressToHostnameMap.at(address).c_str();
}