#ifndef __P2P_APPLICATION_H_
#define __P2P_APPLICATION_H_

#include "inet/networklayer/common/L3Address.h"
#include "s2f/apps/AppBase.h"
#include "s2f/architecture/net/protocol/P2PMsg_m.h"
#include "s2f/messages/Syscall_m.h"
#include <omnetpp.h>
#include <vector>

/**
 * P2P Network peer information.
 */
struct NetworkPeer
{
    int sockFd;            //!< Active TCP connection to the peer
    L3Address ip;          //!< Peer address
    int reconnections = 3; //!< Number of times to attempt connection to peer
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
    const char *dnsSeed{};                   //!< DNS A record seed
    std::vector<NetworkPeer> peers;          //!< Active network peers
    std::vector<NetworkPeer> peerCandidates; //!< Discovery candidates

    L3Address localIp;   //!< Local address
    L3Address dnsIp;     //!< DNS address
    int listeningPort;   //!< Mainnet, Testnet or Regtest port
    int listeningSocket; //!< TCP socket for p2p requesting

    simtime_t simStartTime; //!< Simulation Starting timestamp
    time_t runStartTime;    //!< Real execution time

    virtual void processSelfMessage(cMessage *msg) override;
    virtual void handleResolutionFinished(const L3Address ip,
                                          bool resolved) override;

    /**
     * Handles the initial connection to another peer candidate in the network.
     *
     * @param sockFd    connection file descriptor. Should always match the last
     *                  element in the peer candidates vector.
     * @param connected connection status.
     */
    virtual void handleConnectReturn(int sockFd, bool connected) override;
    virtual void initialize() override;
    virtual void finish() override;

    /**
     * Registers a record in the DNS Seed with the node's IP address.
     */
    void registerServiceToDNS();

    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override
    {
    }
    virtual void returnRead(simtime_t timeElapsed) override {}
    virtual void returnWrite(simtime_t timeElapsed) override {}
    virtual void handleDataArrived(int sockFd, Packet *p) override;
    virtual void handleParameterChange(const char *parameterName) override {}
    virtual bool handlePeerClosed(int sockFd) override { return true; }
    virtual bool handleClientConnection(int sockFd, const L3Address &remoteIp,
                                        const uint16_t &remotePort) override;
};
#endif
