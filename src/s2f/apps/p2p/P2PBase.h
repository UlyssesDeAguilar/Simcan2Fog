#ifndef __P2P_APPLICATION_H_
#define __P2P_APPLICATION_H_

#include "inet/networklayer/common/L3Address.h"
#include "s2f/apps/AppBase.h"
#include "s2f/messages/Syscall_m.h"
#include <omnetpp.h>
#include <vector>

/**
 * @class P2PBase P2PBase.h "P2PBase.h"
 *
 * Base application for the peer-to-peer protocol with various node discovery
 * methods.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-09-08
 */
class P2PBase : public AppBase, public AppBase::ICallback
{
  protected:
    const char *dnsSeed{};                 //!< DNS A record seed
    std::map<int, L3Address> peers;        //!< Active network peers
    std::vector<L3Address> peerCandidates; //!< Discovery candidates

    std::map<int, ChunkQueue> connectionQueue;

    L3Address localIp;   //!< Local address
    L3Address dnsIp;     //!< DNS address
    int listeningPort;   //!< Mainnet, Testnet or Regtest port
    int listeningSocket; //!< TCP socket for p2p requesting

    simtime_t simStartTime; //!< Simulation Starting timestamp
    time_t runStartTime;    //!< Real execution time

    // --------------------------------------------------------------------- //
    //                           P2PBASE METHODS                             //
    // --------------------------------------------------------------------- //

    /**
     * Callback for the failure-case scenario when connecting to a peer node.
     *
     * A failure can be triggered either on the connection or due to protocol
     * incompatibilities.
     */
    virtual void handleConnectFailure(int sockFd);

    /**
     * Registers a record in the DNS Seed with the node's IP address.
     */
    void registerServiceToDNS();

    // --------------------------------------------------------------------- //
    //                          APPBASE OVERRIDES                            //
    // --------------------------------------------------------------------- //

    /**
     * Initialization hook for this module.
     */
    virtual void initialize() override;

    /**
     * Finish hook that runs when the simulation is terminated without errors.
     */
    virtual void finish() override;

    /**
     * Handle hook for messages sent by this module.
     *
     * @param msg   Message to process.
     */
    virtual void processSelfMessage(cMessage *msg) override;

    /**
     * Handle hook for resolutions sent by the DNS.
     *
     * @param ip        IP Address received on a successful resolution.
     * @param resolved  Resolution status.
     */
    virtual void handleResolutionFinished(const L3Address ip,
                                          bool resolved) override;
    /**
     * Handle hook for socket connection initiated by a possible peer.
     *
     * @param sockFd    File descriptor for this connection.
     * @param remoteIp  Peer Ip address.
     */
    virtual bool handleClientConnection(int sockFd, const L3Address &remoteIp,
                                        const uint16_t &remotePort) override;

    /**
     * Handle hook for peer disconnection.
     *
     * @param sockFd    File descriptor for this connection.
     */
    virtual bool handlePeerClosed(int sockFd) override;

    // --------------------------------------------------------------------- //
    //                          CHILDREN OVERRIDES                           //
    // --------------------------------------------------------------------- //

    /**
     * Handles the initial connection to another peer candidate in the network.
     *
     * @param sockFd    connection file descriptor.
     * @param connected connection status.
     */
    virtual void handleConnectReturn(int sockFd, bool connected) override {}

    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override
    {
    }
    virtual void returnRead(simtime_t timeElapsed) override {}
    virtual void returnWrite(simtime_t timeElapsed) override {}
    virtual void handleDataArrived(int sockFd, Packet *p) override {}
    virtual void handleParameterChange(const char *parameterName) override {}
};
#endif
