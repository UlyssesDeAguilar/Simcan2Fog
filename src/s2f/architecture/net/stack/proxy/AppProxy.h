/**
 * @file AppProxy.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief This file contains the definition of the AppProxy module
 * @version 1.0
 * @date 2025-02-02
 * 
 */
#ifndef SIMCAN_EX_APP_PROXY
#define SIMCAN_EX_APP_PROXY

#include "inet/common/INETDefs.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "s2f/architecture/net/stack/proxy/ProxyServiceRequest_m.h"
#include "s2f/architecture/net/stack/proxy/ServiceTable.h"
#include "s2f/architecture/net/stack/proxy/Session.h"
#include "s2f/architecture/net/protocol/RestfulRequest_m.h"

/**
 * @brief Defines the connection type
 */
enum ConnectionType
{
    PASSTHROUGH, //!< The communication is pass-through, for client connections
    EDGE         //!< The communication is edge terminated, for server connections
};

/**
 * @brief This struct manages the Connection information and state
 */
struct Connection
{
    int socketId;        //!< Socket id of the endpoint
    int gateIndex;       //!< Gate index of the endpoint
    int ip = 0;          //!< Declared IP address of the endpoint
    int port = 0;        //!< Declared port of the endpoint
    ConnectionType type; //!< Connection type

    friend std::ostream &operator<<(std::ostream &os, const Connection &obj)
    {
        return os << "Socket: " << obj.socketId << " | Gate: " << obj.gateIndex << " | Type: " << obj.typeToString();
    }

    /**
     * @brief Returns the connection type as a string representation
     * @return const char* The string
     */
    const char *typeToString() const
    {
        if (type == PASSTHROUGH)
            return "PASSTHROUGH";
        else
            return "EDGE";
    }
};

/**
 * @brief This module is a proxy for the applications, managing the incoming and outgoing connections
 */
class AppProxy : public omnetpp::cSimpleModule, public inet::LifecycleUnsupported
{
protected:
    std::map<int, Connection> socketToConnnectionMap; //!< Maps socket id to Connections
    std::map<int, Session> socketToSessionMap;        //!< Maps socket id to Sessions
    ServiceTable *serviceTable = nullptr;             //!< Pointer to the service table
    inet::TcpSocket serverSocket;                     //!< Server socket, where the incoming connections are accepted
    int inGateBase;                                   //!< Base id of the input gate from the applications
    int outGateBase;                                  //!< Base id of the output gate to the applications
    int transportGateIn;                              //!< Id of the transport input gate
    int transportGateOut;                             //!< Id of the transport output gate

    // Kernel lifecycle
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;

    /**
     * @brief Handles the application layer (or upper layer) messages
     * @param msg The message to be processed
     */
    void handleUpperMessage(omnetpp::cMessage *msg);

    /**
     * @brief Handles the transportation layer (or lower layer) messages
     * @param msg The message to be processed
     */
    void handleLowerMessage(omnetpp::cMessage *msg);

    /**
     * @brief Handles service registrations and unregistrations from endpoints
     * @param packet The packet to be processed
     * @param serviceRequest The request
     */
    void handleServiceRequest(inet::Packet *packet, const ProxyServiceRequest *serviceRequest);

    /**
     * @brief Manages the application data packet
     * @param packet The packet to be sent
     */
    void handleAppPacket(inet::Packet *packet);

    /**
     * @brief Handles application TCP commands
     * @param message The message to be processed
     */
    void handleAppCommand(inet::Message *message);

    /**
     * @brief Manages indications from the transport layer
     * @param message Message containing the indication
     */
    void handleTransportIndication(inet::Message *message);

    /**
     * @brief Manages packets from the transport layer
     * @param packet The packet to be forwarded 
     */
    void handleTransportPacket(inet::Packet *packet);

    /**
     * @brief Selects randomly (uniform distribution)an IP based on the entries
     * @param entries The entries of the service table
     * @return int The index of the selected entry
     */
    int selectIp(std::vector<ServiceEntry> &entries);

    /**
     * @brief Retrieves the service name from the application packets
     * @param packet The packet to be analyzed
     * @return std::string The service name or an empty string
     */
    std::string getServiceName(inet::Packet *packet);

    /**
     * @brief Attempts to find the gate index of a socket
     *
     * @param socketId The socket id
     * @return int The index of the respective output gate
     */
    int findSocketGateIndex(int socketId);

    /**
     * @brief Finds a connection based on the IP and port
     *
     * @param ip IP address
     * @param port Port
     * @return Connection* The object or nullptr if not found
     */
    Connection *findConnection(int ip, int port);
};

#endif // SIMCAN_EX_APP_PROXY
