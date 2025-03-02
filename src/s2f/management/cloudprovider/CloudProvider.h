#ifndef SIMCAN_EX_CLOUDPROVIDER_H_
#define SIMCAN_EX_CLOUDPROVIDER_H_

#include <algorithm>
#include <omnetpp.h>

#include "s2f/core/simdata/DataManager.h"
#include "s2f/management/cloudprovider/NodeUpdate_m.h"
#include "s2f/management/cloudprovider/NodeDb.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/messages/SM_UserAPP.h"
#include "s2f/messages/SM_CloudProvider_Control_m.h"

/**
 * @brief This class models the behaviour of a cloud provider
 * @details It will attempt to allocate VMs to users by redirecting them to the right node
 */
class CloudProvider : public cSimpleModule
{
protected:
    struct QueueElement
    {
        SM_UserVM *request;
        cMessage *timeOut;
        QueueElement(SM_UserVM *r, cMessage *to) : request(r), timeOut(to) {}
    };

    using RequestQueue = std::deque<QueueElement>;

    DataManager *dataManager{}; //!< Reference to the data manager
    NodeDb *nodeDb{};           //!< Reference to the node database
    RequestQueue queue;         //!< Queue of the requests that are waiting to be handled
    bool dispatchPriority;      //!< Whether priority users skip the queue or not
    const char *defaultZone;    //!< Default zone where to dispatch the requests

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    /**
     * @brief Handle a node update or registration
     */
    void handleNodeUpdate(NodeUpdate *update);

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
     * @brief Selects and allocates a node for the request
     * @param request The request to examine
     * @return const char* The topic if sucessfull or nullptr in other case
     */
    const char *selectNode(const SM_UserVM *request);

    /**
     * @brief Selects and allocates a node from a node pool for the request
     * 
     * @param request The request to examine
     * @param pool The pool where to allocate the resources
     * @return const char* The topic if sucessfull or nullptr in other case
     */
    const char *selectNodeInPool(const SM_UserVM *request, const std::set<int> &pool);

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
