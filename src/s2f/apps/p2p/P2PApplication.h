#ifndef __P2P_APPLICATION_H_
#define __P2P_APPLICATION_H_

#include "inet/networklayer/common/L3Address.h"
#include "s2f/apps/AppBase.h"
#include "s2f/architecture/net/protocol/P2PMsg_m.h"
#include <omnetpp.h>
#include <vector>

class NetworkPeer
{
  public:
    L3Address ip;
    int sockFd;
    NetworkPeer(L3Address ip, int sockFd)
    {
        ip = ip;
        sockFd = sockFd;
    }
};
/**
 * @class P2PApplication P2PApplication.h "P2PApplication.h"
 *
 * Base application for the peer-to-peer protocol with various node discovery
 * methods.
 *
 * @author Tomás Daniel Expósito override
 * @date 2025-09-08
 */
class P2PApplication : public AppBase, public AppBase::ICallback
{
  protected:
    enum ActiveNetwork
    {
        MAIN_NET = 8333,
        TEST_NET = 18333,
        REG_NET = 18444
    };
    enum State
    {
        DISCOVERY_PERSISTENT,
        DISCOVERY_DNSSEED,
        DISCOVERY_FALLBACKIPS,
        END
    } state;

    const char *dnsSeed{}; //!< DNS A record seed

    std::vector<NetworkPeer> peers;          //!< Active network peers
    std::vector<NetworkPeer> fallbackPeers;  //!< Known, hardcoded peers
    std::vector<NetworkPeer> peerCandidates; //!< Candidates for p2p discovery

    int listeningPort;   //!< Mainnet, Testnet or Regtest port
    int listeningSocket; //<! TCP socket for p2p requesting

    simtime_t simStartTime; //!< Simulation Starting timestamp
    time_t runStartTime;    //!< Real execution time

    virtual void processSelfMessage(cMessage *msg) override;
    virtual void handleParameterChange(const char *parameterName) override;
    virtual void initialize() override;
    virtual void finish() override;

    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override;
    virtual void returnRead(simtime_t timeElapsed) override {}
    virtual void returnWrite(simtime_t timeElapsed) override {}
    virtual void handleDataArrived(int sockFd, Packet *p) override;
    virtual void handleConnectReturn(int sockFd, bool connected) override;
    virtual bool handlePeerClosed(int sockFd) override;
    virtual void handleResolutionFinished(const L3Address ip,
                                          bool resolved) override;
};
#endif
