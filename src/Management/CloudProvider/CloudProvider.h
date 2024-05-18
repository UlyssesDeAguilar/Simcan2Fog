#ifndef SIMCAN_EX_CLOUDPROVIDER_H_
#define SIMCAN_EX_CLOUDPROVIDER_H_

#include <algorithm>
#include <omnetpp.h>
#include "inet/common/packet/Packet.h"
#include "Core/DataManager/DataManager.h"
#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/CloudProvider/NodeEvent_m.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"
#include "Messages/SM_CloudProvider_Control_m.h"

/**
 * Class that parses information about the data-centres.
 *
 */
class CloudProvider : public cSimpleModule
{

protected:
    /**
     * @brief Keeps track of registered resources for the cloud provider
     * @details In the future it could hold more information and help tracking for availability (heart-beat)
     */
    struct Node
    {
        uint64_t availableCores;
        opp_string topic;
    };

    struct QueueElement
    {
        SM_UserVM *request;
        cMessage *timeOut;
        QueueElement(SM_UserVM *r, cMessage *to) : request(r), timeOut(to) {}
    };

    using NodePool = std::vector<Node>;
    using NodeCoresMap = std::map<opp_string, Node *>;
    using CoresNodeMap = std::map<uint64_t, std::set<Node *>>;
    using RequestQueue = std::deque<QueueElement>;

    NodePool nodePool;        //!< Holds the registered nodes and their status
    DataManager *dataManager; //!< Reference to the data manager
    NodeCoresMap nodeMap;     //!< Keeps track of the node (topic) - available cores relation
    CoresNodeMap coresMap;    //!< Keeps a set of nodes (topic) of related core counts
    RequestQueue queue;       //!< Queue of the requests that are waiting to be handled
    bool dispatchPriority;    //!< Whether priority users skip the queue or not
    int listenerGate;         //!< Holds the id of the listener gate

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    /**
     * @brief Handle a node update or registration
     */
    void handleNodeUpdate(inet::Packet *update);

    /* Handler methods*/

    /**
     * Handles if a VM request fits in the datacentre.
     * @param userVM_Rq Virtual machine request.
     */
    void handleVmRequest(SM_UserVM *sm);

    /**
     * Handles a VM subscription
     * @param userVM_Rq User VM request
     */
    void handleVmSubscription(SM_UserVM *sm);

    /**
     * Manages a subscription timeout.
     * @param msg Message timeout
     */
    void handleSubscriptionTimeout(cMessage *msg);

    /* Helper methods*/

    /**
     * Rejects the user request.
     * @param userVM_Rq User request.
     */
    void rejectVmRequest(SM_UserVM *userVM_Rq);

    /**
     * @brief Attempts dispatching requests stored in the queue
     */
    void attemptDispatching();

    /**
     * @brief Selects a node from the pool which will satisfy the request
     * @details It also updates the resource in the selected node accordingly
     * @param request The request to examine
     * @return const opp_string* The topic if sucessfull or nullptr in other case
     */
    const opp_string *selectNode(const SM_UserVM *request);

    /**
     * @brief Recovers the user type by it's id
     * @param userId The user id
     * @return const CloudUser* The reference to the user definition type
     */
    const CloudUser *findUserTypeById(const std::string &userId);

    /**
     * @brief Calculates the number of cores needed by the request
     * @param request The request
     * @return uint64_t The total number of cores needed
     */
    uint64_t calculateRequestedCores(const SM_UserVM *request);
};

#endif
