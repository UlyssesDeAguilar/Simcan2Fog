/**
 * @file Session.h
 * @author Ulysses de Aguilar Gudmundsson
 * @brief Defines the Session class, used mainly in the AppProxy module
 * @version 1.0
 * @date 2025-02-02
 * 
 */
#ifndef SIMCAN_EX_SESSION
#define SIMCAN_EX_SESSION

#include "omnetpp.h"
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Message.h"

/**
 * @brief Represents the state of a session
 */
enum SessionState
{
    INIT,       //!< TCP connection established, endpoint not inferred yet
    PENDING,    //!< Endpoint connection not yet established fully
    ESTABLISHED //!< Endpoint connection established
};

/**
 * @brief This class manages the context and lifecycle of a user session
 */
class Session
{
    int socketId;                            //!< The socket id of the endpoint
    int gateIndex = -1;                      //!< The gate index of the endpoint
    SessionState state = INIT;               //!< The state of the session
    inet::Message *socketInd = nullptr;      //!< Must be the original TCP_I_AVAILABLE intercepted
    omnetpp::cQueue *pendingQueue = nullptr; //!< The queue of pending messages (while on PENDING state)

public:
    /**
     * @brief Construct a new Session object
     *
     * @param socketId The socket id
     * @param socketInd The TCP_I_AVAILABLE intercepted
     */
    Session(int socketId, inet::Message *socketInd) : socketId(socketId), socketInd(socketInd) {}

    /**
     * @brief Destroy the Session object
     */
    virtual ~Session();

    /**
     * @brief Push a message to the pending queue
     * @param msg The messsage to push
     */
    void pushToPendingQueue(omnetpp::cMessage *msg);

    /**
     * @brief Set the Session Pending object
     * @param gateIndex The gate index of the endpoint
     * @return inet::Message* The TCP_I_AVAILABLE to be sent to the endpoint, now it's the users responsibility to delete
     */
    inet::Message *setSessionPending(int gateIndex);

    /**
     * @brief Set the Session to ESTABLISHED
     * @return omnetpp::cQueue* The pending message queue, now it's the users responsibility to delete
     */
    omnetpp::cQueue *setSessionEstablished();

    /**
     * @brief Get the endpoint gate index
     * @return int The gate index
     */
    int getGateIndex() { return gateIndex; }

    /**
     * @brief Get the actual state of the Session
     * @return SessionState The state
     */
    SessionState getState() { return state; }
};

#endif // SIMCAN_EX_SESSION