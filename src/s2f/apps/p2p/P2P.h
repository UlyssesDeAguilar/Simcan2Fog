#ifndef __P2P_PROTOCOL_H__
#define __P2P_PROTOCOL_H__

#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/checkandcast.h"
#include "s2f/apps/AppBase.h"
#include "s2f/apps/p2p/Peer_m.h"
#include "s2f/messages/Syscall_m.h"
#include <omnetpp.h>
#include <vector>

/**
 * @class P2P P2P.h "P2P.h"
 *
 * Base application for the peer-to-peer protocol.
 * Manages all existing connextions to the peer
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-09-08
 */
class P2P : public AppBase, public AppBase::ICallback
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

    std::map<int, Peer *> peerData; //!< Peer data
    std::set<L3Address> peers;      //<! Active peers
    L3Address localIp;              //!< Local address
    int listeningPort;              //!< Protocol port
    int listeningSocket;            //!< TCP socket for p2p requesting

    cGate *discoveryService{};
    std::set<L3Address> resolutionList; //!< Addresses obtained through discovery services
    int discoveryAttempts;              //!< Number of times to run discovery services
    int discoveryThreshold;             //!< Minimum number of peers to konw

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
    virtual int getSockFd(L3Address ip);

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
    virtual void initialize(int stage) override;

    /**
     * Finish hook that runs when the simulation is terminated without errors.
     */
    virtual void finish() override;

    virtual void handleMessage(cMessage *msg) override;

    /**
     * Handle hook for messages sent by this module.
     *
     * @param msg   Message to process.
     */
    virtual void processSelfMessage(cMessage *msg) override;

    /**
     * Handle hook for peer disconnection.
     *
     * @param sockFd    Connection file descriptor.
     */
    virtual bool handlePeerClosed(int sockFd) override;

    virtual int numInitStages() const override { return INITSTAGE_APPLICATION_LAYER + 1; }
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

    virtual void handleConnectReturn(int sockFd, bool connected) override {};
    virtual void handleDataArrived(int sockFd, Packet *p) override {};
    virtual void handleResolutionFinished(const L3Address ip, bool resolved) override {};
    virtual void handleResolutionFinished(const std::set<L3Address> ipResolutions, bool resolved) override {};
    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override {};
    virtual void returnRead(simtime_t timeElapsed) override {};
    virtual void returnWrite(simtime_t timeElapsed) override {};
    virtual void handleParameterChange(const char *parameterName) override {};
};
#endif
