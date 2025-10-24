#ifndef __P2P_APPLICATION_H_
#define __P2P_APPLICATION_H_

#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/checkandcast.h"
#include "s2f/apps/AppBase.h"
#include "s2f/architecture/p2p/pow/PowMsgAddress_m.h"
#include "s2f/messages/Syscall_m.h"
#include <omnetpp.h>
#include <vector>

using namespace s2f::p2p;

/**
 * @class P2PBase P2PBase.h "P2PBase.h"
 *
 * Base application for the peer-to-peer protocol.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-09-08
 */
class P2PBase : public AppBase, public AppBase::ICallback
{
  protected:
    enum P2PEvent
    {
        NODE_UP = 4,     //!< Post-startup action for protocol implementation
        PEER_DISCOVERY,  //!< Peer discovery handling for protocol implementation
        PEER_CONNECTION, //!< Connection to peer candidates from discovery process
        CONNECTED        //!< Node is ready and connected to the network
    };
    const char *dnsSeed{};                 //!< DNS A record seed
    std::map<int, NetworkPeer *> peers;    //!< Active network peers
    std::vector<L3Address> peerCandidates; //!< Discovery candidates

    std::map<int, ChunkQueue> connectionQueue;

    L3Address localIp;   //!< Local address
    L3Address dnsIp;     //!< DNS address
    int listeningPort;   //!< Protocol port
    int listeningSocket; //!< TCP socket for p2p requesting
    int dnsSock;         //!< Active DNS connection

    int discoveryAttempts;
    int discoveryThreshold;

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
     * Wrapper method to create a connection to a new peer.
     */
    virtual void connectToPeer();

    virtual int findIpInPeers(L3Address ip);

    virtual bool isClient(int sockFd) { return listeningPort == check_and_cast<TcpSocket *>(socketMap.getSocketById(sockFd))->getLocalPort(); }
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
    virtual void handleResolutionFinished(const L3Address ip, bool resolved) override {};
    virtual void handleResolutionFinished(const std::set<L3Address> ipResolutions, bool resolved) override;

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
     * Handles connections related to the DNS service.
     * Peer connections should be handled by specific protocol implementations.
     *
     * @param sockFd    connection file descriptor.
     * @param connected connection status.
     */
    virtual void handleConnectReturn(int sockFd, bool connected) override;

    /**
     * Handles packets arrived from an existing connection to the DNS service.
     * Peer connections should be handled by specific protocol implementations.
     *
     * @param sockFd    connection file descriptor.
     * @param p         connection incoming data.
     */
    virtual void handleDataArrived(int sockFd, Packet *p) override;

    /**
     * Handle hook for socket connection initiated by a possible peer.
     *
     * @param sockFd    File descriptor for this connection.
     * @param remoteIp  Peer Ip address.
     */
    virtual bool handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort) override { return true; }

    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override {}
    virtual void returnRead(simtime_t timeElapsed) override {}
    virtual void returnWrite(simtime_t timeElapsed) override {}
    virtual void handleParameterChange(const char *parameterName) override {}
};
#endif
