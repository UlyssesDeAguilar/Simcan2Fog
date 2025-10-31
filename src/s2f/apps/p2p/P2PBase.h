#ifndef __P2P_APPLICATION_H__
#define __P2P_APPLICATION_H__

#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/checkandcast.h"
#include "s2f/apps/AppBase.h"
#include "s2f/apps/p2p/NetworkPeer_m.h"
#include "s2f/messages/Syscall_m.h"
#include <omnetpp.h>
#include <vector>

/**
 * @class P2PBase P2PBase.h "P2PBase.h"
 *
 * Base application for the peer-to-peer protocol.
 * Manages all existing connextions to the peer
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
        PEER_CONNECTION, //!< Connection to peer candidates from discovery
                         //!< process
        CONNECTED        //!< Node is ready and connected to the network
    };

    const char *dnsSeed{};              //!< DNS A record seed
    std::map<int, NetworkPeer *> peers; //!< Active network peers
    std::vector<L3Address>
        resolutionList;  //!< Addresses obtained through discovery services
    L3Address localIp;   //!< Local address
    L3Address dnsIp;     //!< DNS address
    int listeningPort;   //!< Protocol port
    int listeningSocket; //!< TCP socket for p2p requesting
    int dnsSock;         //!< Active DNS connection

    int discoveryAttempts;  //!< Number of times to run discovery services
    int discoveryThreshold; //!< Number of nodes before considering oneself
                            //!< connected

    simtime_t simStartTime; //!< Simulation Starting timestamp
    time_t runStartTime;    //!< Real execution time

    // --------------------------------------------------------------------- //
    //                                METHODS                                //
    // --------------------------------------------------------------------- //

    /**
     * Callback for the failure-case scenario when connecting to a peer node.
     *
     * A failure can be triggered either on the connection or due to protocol
     * incompatibilities.
     *
     * @param sockFd    Connection descriptor.
     */
    virtual void handleConnectFailure(int sockFd);

    /**
     * Wrapper method to create a connection to a new peer.
     */
    virtual void connectToPeer();

    /**
     * Finds the sockFd associated to an ip address.
     *
     * @param address   Peer address.
     * @return sockFd on found peer, 0 otherwise.
     */
    virtual int findIpInPeers(L3Address ip);

    /**
     * Determines whether this node started the connection or not.
     *
     * @param sockFd    Connection descriptor.
     * @return true if this node starts the connection, false otherwise.
     */
    virtual bool isClient(int sockFd)
    {
        return listeningPort != check_and_cast<TcpSocket *>(socketMap.getSocketById(sockFd))->getLocalPort();
    }

    // --------------------------------------------------------------------- //
    //                                OVERRIDES                              //
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

    /**
     * Handle hook for resolutions sent by the DNS.
     *
     * @param ipResolutions IP Address list received on successful resolution.
     * @param resolved      Resolution status.
     */
    virtual void handleResolutionFinished(const std::set<L3Address> ipResolutions, bool resolved) override;

    /**
     * Handle hook for peer disconnection.
     *
     * @param sockFd    Connection file descriptor.
     */
    virtual bool handlePeerClosed(int sockFd) override;

    /**
     * Handles connections related to the DNS service.
     * Peer connections should be handled by specific protocol implementations.
     *
     * @param sockFd    Connection file descriptor.
     * @param connected Connection status.
     */
    virtual void handleConnectReturn(int sockFd, bool connected) override;

    /**
     * Handles packets arrived from an existing connection to the DNS service.
     * Peer connections should be handled by specific protocol implementations.
     *
     * @param sockFd    Connection file descriptor.
     * @param p         Connection incoming data.
     */
    virtual void handleDataArrived(int sockFd, Packet *p) override;

    /**
     * Handle hook for socket connection initiated by a possible peer.
     *
     * @param sockFd        File descriptor for this connection.
     * @param remoteIp      Peer Ip address.
     * @param remotePort    Peer port.
     */
    virtual bool handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort) override
    {
        return true;
    }

    // --------------------------------------------------------------------- //
    //                          CHILDREN OVERRIDES                           //
    // --------------------------------------------------------------------- //

    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override {}
    virtual void returnRead(simtime_t timeElapsed) override {}
    virtual void returnWrite(simtime_t timeElapsed) override {}
    virtual void handleParameterChange(const char *parameterName) override {}
};
#endif
