#ifndef __P2P_DISCOVERY_H__
#define __P2P_DISCOVERY_H__

#include "inet/common/InitStages.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3Address.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/csimplemodule.h"
#include "s2f/apps/AppBase.h"
#include "s2f/architecture/p2p/pow/PowCommon.h"
#include "s2f/architecture/p2p/pow/PowPeer_m.h"
#include "s2f/architecture/p2p/pow/messages/Header_m.h"
#include <omnetpp.h>
#include <vector>

/**
 * @class DnsDiscoveryService DnsDiscveryService.h "DnsDiscoveryService.h"
 *
 * Discovery service base class for the peer-to-peer protocol.
 *
 * @author Tomás Daniel Expósito Torre
 * @date 2025-10-29
 */
class DnsDiscoveryService : public AppBase, public AppBase::ICallback
{
  private:
    inet::L3Address dnsIp; //!< DNS address
    const char *dnsSeed{}; //!< DNS A record seed
    int dnsSock;           //!< Active DNS connection
  public:
    /**
     * Initialization hook for this module.
     *
     * @param stage Initialization stage.
     */
    virtual void initialize(int stage) override;

    /**
     * Handler for received messages.
     */
    virtual void handleMessage(omnetpp::cMessage *msg) override;

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
     * Handles connections related to the DNS service.
     * Peer connections should be handled by specific protocol implementations.
     *
     * @param sockFd    Connection file descriptor.
     * @param connected Connection status.
     */
    virtual void handleConnectReturn(int sockFd, bool connected) override;

    virtual int numInitStages() const override { return INITSTAGE_APPLICATION_LAYER + 1; }

    /**
     * Handles packets arrived from an existing connection to the DNS service.
     * Peer connections should be handled by specific protocol implementations.
     *
     * @param sockFd    Connection file descriptor.
     * @param p         Connection incoming data.
     */
    virtual void handleDataArrived(int sockFd, Packet *p) override;

    virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) override {};
    virtual void returnRead(simtime_t timeElapsed) override {};
    virtual void returnWrite(simtime_t timeElapsed) override {};
    virtual void handleParameterChange(const char *parameterName) override {};
    virtual bool handlePeerClosed(int sockFd) override { return true; };
    virtual void processSelfMessage(cMessage *msg) override {};
};
#endif
