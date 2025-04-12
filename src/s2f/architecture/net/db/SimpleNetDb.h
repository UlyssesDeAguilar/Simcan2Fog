#ifndef SIMCAN_EX_SIMPLENETDB_H__
#define SIMCAN_EX_SIMPLENETDB_H__

#include <omnetpp.h>

class SimpleNetDb : public omnetpp::cSimpleModule
{
protected:
    std::map<omnetpp::opp_string, int> hostnameToAddressMap;
    std::map<int, omnetpp::opp_string> addressToHostnameMap;

    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;

public:
    void registerHost(const char *hostName, int address);
    int getAddress(const char *hostName);
    const char *getHostName(int address);
};

#endif /* SIMCAN_EX_SIMPLENETDB_H__ */