#ifndef CLOUDUSERINSTANCE_H_
#define CLOUDUSERINSTANCE_H_

#include "UserInstance.h"
#include "Management/dataClasses/VirtualMachines/VmInstance.h"
#include "CloudUser.h"

// #include "Messages/VMRequest.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"

struct InstanceTimes
{
    int64_t initTime;      // User intialization time (generated according to the distribution)
    int64_t arrival2Cloud; // The arrival to the cloud (generally equals initTime)
    int64_t initWaitTime;  // Start of wait for resources
    int64_t waitTime;      // The time elapsed during the wait
    int64_t initExec;      // Start of execution of Apps
    int64_t endTime;       // When the user requests finally where satisfied

    int64_t convertToHours(const int64_t &val) const
    {
        return val != 0 ? val / 3600 : 0;
    }

    InstanceTimes convertToHours() const
    {
        return {
            .initTime = convertToHours(this->initTime),
            .arrival2Cloud = convertToHours(this->arrival2Cloud),
            .initWaitTime = convertToHours(this->initWaitTime),
            .waitTime = convertToHours(this->waitTime),
            .initExec = convertToHours(this->initExec),
            .endTime = convertToHours(this->endTime)};
    }
};

/**
 * Class that represent a modeled user in SIMCAN for cloud environments.
 *
 * Basically, the instance of one user contains:
 *  - A collection of applications to be executed.
 *  - A collection of VMs where these applications must be executed.
 */
class CloudUserInstance : public UserInstance
{

private:
    int nId;                                       // Numerical id
    VM_Request::InstanceRequestTimes requestTimes; // Request times or maximum renting times
    InstanceTimes times;                           // Times of specific events
    group_vector<VmInstanceType, VmInstance>
        vmGroupedInstances;            //!< This keeps the extrinsic state grouped by the flat version for the vmInstances
    std::map<opp_string, int> vmIdMap; // Map that coorelates a VmId to it's own instance state
    int numFinishedVMs;                // Number of finished vm requests
    int numActiveSubscriptions;        // Number of active subscriptions

protected:
    SM_UserVM *requestVmMsg{};

    // Flags that describe inner state FIXME: Create a true state machine!
    bool bTimeout_t2;
    bool bTimeout_t4;
    bool bFinished;
    bool bSubscribe;

    /**
     * @brief Calculates the number of instances requested by a User of a specific type
     * @param user   The user reference
     * @return int   The number of instances required
     */
    int getNumVms(const CloudUser *user);

public:
    /**
     * Constructor.
     *
     * Generates a unique name for this user instance using the following syntax: (<i>userNumber</i>)<i>userType</i>[<i>currentInstanceIndex</i>/<i>totalUserInstances</i>
     *
     * @param ptrUser Pointer to the user object that contains the applications and VMs to be generated.
     * @param userNumber Order in which this user has been created.
     * @param currentInstanceIndex User instance. First instance of this user must be 0.
     * @param totalUserInstances Total number of user instances to be created for this <b>userNumber</b>.
     */
    CloudUserInstance(const CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances);

    bool operator<(const CloudUserInstance &other) const;

    /**
     * Converts this user instance in string format.
     *
     * @param includeAppsParameters If set to \a true, the generated string will contain the parameters of each application in the <b>applications</b> vector.
     * @param includeVmFeatures If set to \a true, the generated string will contain the features of each VM in the <b>virtualMachines</b> vector.
     *
     * @return String containing this cloud user instance.
     */
    virtual std::string toString(bool includeAppsParameters, bool includeVmFeatures);

    /**
     * @brief FIXME: Figure out exactly what does this mean!
     * @return int The presumed numerical ID
     */
    int getNId() const { return nId; }

    /**
     * Gets the nth vm in the virtualmachine list of collections flattened.
     * @param index Index of the required vm.
     */
    const VmInstance &getVmInstance(int index) { return vmGroupedInstances.flattened().at(index); }
    const std::vector<VmInstance> &getAllVmInstances() { return vmGroupedInstances.flattened(); }
    const group_vector<VmInstanceType, VmInstance> &getAllVmInstanceTypes() const { return vmGroupedInstances; }
    
    // Managing the messages sent and received by the user.
    SM_UserVM *getRequestVmMsg() { return requestVmMsg; }
    void setRequestVmMsg(SM_UserVM *requestVmMsg) { this->requestVmMsg = requestVmMsg; }

    /**
     * @brief Set the Rent Times object
     * @param requestTimes The specifics of the renting times
     */
    void setRentTimes(const VM_Request::InstanceRequestTimes &requestTimes) { this->requestTimes = requestTimes; }

    /**
     * @brief Get the Rent Times object
     * @return Constant reference to the Rent Times
     */
    const VM_Request::InstanceRequestTimes &getRentTimes() { return requestTimes; }

    /**
     * @brief Gets the instance times
     * @return Constant reference to the times
     */
    const InstanceTimes &getInstanceTimes() { return times; }

    /**
     * @brief Get the Instance Times For Update object
     * @return A mutable reference to the Instance times
     */
    InstanceTimes &getInstanceTimesForUpdate() { return times; }

    /**
     * @brief Whether the user finished or not
     */
    bool isFinished() const { return bFinished; }

    /**
     * @brief Whether the user timed out for renting time
     */
    bool isTimeoutMaxRent() const { return bTimeout_t2; }

    /**
     * FIXME: Figure out if this is true!
     * @brief hether the user timed out for waiting for a subscription
     */
    bool isTimeoutSubscribed() const { return bTimeout_t4; }

    /**
     * @brief If either timeout happened
     */
    bool isTimeout() const { return (bTimeout_t2 || bTimeout_t4); }

    void setTimeoutMaxStart() { bTimeout_t2 = true; }
    void setTimeoutMaxRentTime() { bTimeout_t2 = true; }
    void setTimeoutMaxSubscribed() { bTimeout_t4 = true; }

    tVmState getVmInstanceState(const opp_string &r) { return vmGroupedInstances.flattened().at(vmIdMap.at(r)).getState();}
    int getTotalVMs() const { return vmGroupedInstances.flattened().size(); }

    /**
     * @brief If all of the rented vms ended their contract
     */
    bool allVmsFinished() const { return getTotalVMs() <= numFinishedVMs; }

    /**
     * @brief Wheter the user did subscribe or not
     */
    bool hasSubscribed() const { return bSubscribe; }

    void startExecution();
    void startSubscription();
    void endSubscription();
    void updateVmInstanceStatus(const char *vmId, tVmState state);
    void updateVmInstanceStatus(const SM_UserVM* request, tVmState state);
    void finish();
};

#endif /* USERINSTANCE_H_ */
