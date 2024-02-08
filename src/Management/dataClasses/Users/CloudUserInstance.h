#ifndef CLOUDUSERINSTANCE_H_
#define CLOUDUSERINSTANCE_H_

#include "UserInstance.h"
#include "Management/dataClasses/VirtualMachines/VmInstanceCollection.h"
#include "CloudUser.h"

//#include "Messages/VMRequest.h"
#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"

struct InstanceTimes
{
    SimTime initTime;      // User intialization time (generated according to the distribution)
    SimTime arrival2Cloud; // The arrival to the cloud (generally equals initTime)
    SimTime initWaitTime;  // Start of wait for resources
    SimTime waitTime;      // The time elapsed during the wait
    SimTime initExec;      // Start of execution of Apps
    SimTime endTime;       // When the user requests finally where satisfied

    SimTime convertToSeconds(const SimTime &val)
    {
        return val != 0 ? val / 3600 : 0;
    }

    InstanceTimes convertToSeconds()
    {
        return {
            .initTime = convertToSeconds(this->initTime),
            .arrival2Cloud = convertToSeconds(this->arrival2Cloud),
            .initWaitTime = convertToSeconds(this->initWaitTime),
            .waitTime = convertToSeconds(this->waitTime),
            .initExec = convertToSeconds(this->initExec),
            .endTime = convertToSeconds(this->endTime)};
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
    int nId;                                             // Numerical id
    std::vector<VmInstanceCollection *> virtualMachines; // Vector of Virtual Machines configured for this user.
    InstanceTimes times;                                 // Times of specific events
    VM_Request::InstanceRequestTimes requestTimes;       // Request times or maximum renting times
    int numFinishedVMs;                                  // Number of finished vm requests
    int numActiveSubscriptions;                          // Number of active subscriptions

protected:
    int numTotalVMs;                         // All of the vms contained in the collection
    std::vector<AppInstance *> appInstances; // Flattened App vector instance FIXME: Should derive an ADT that optimizes this!
    SM_UserVM *requestVmMsg;
    SM_UserAPP *requestAppMsg;
    SM_UserVM *subscribeVmMsg;

    // Flags that describe inner state FIXME: Create a true state machine!
    bool bTimeout_t2;
    bool bTimeout_t4;
    bool bFinished;
    bool bSubscribe;

    /**
     * Creates the corresponding VMs instances to be included in the <b>virtualMachines</b> vector.
     *
     * @param vmPtr Pointer to the </b>VirtualMachine</b> object that contains the main features of the generated VM.
     * @param numInstances Number of instances of the generated VM.
     */
    void insertNewVirtualMachineInstances(const VirtualMachine *vmPtr, int numInstances, int nRentTime, int total, int offset);

    /**
     * @brief Flattens the AppCollections into a single vector!
     */
    void processApplicationCollection();

    /**
     * @brief Calculates the number of instances requested by a User of a specific type
     *
     * @param vmType Type of a virtual machine
     * @param user   The user reference
     * @return int   The number of instances required
     */
    int getNumVms(std::string vmType, const CloudUser *user);

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

    /**
     * Destructor.
     */
    virtual ~CloudUserInstance();

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
     *  Return the number of virtual machines collections
     */
    int getNumberVmCollections() { return virtualMachines.size(); }

    /**
     * Get an specific Vm collection
     * @param nCollection The ID of virtual machine collection to be retrieved.
     * @throws std::out_of_range If the index is off bounds
     * @return Collection of VM instances.
     */
    VmInstanceCollection *getVmCollection(int nCollection) { return virtualMachines.at(nCollection); }

    /**
     * @brief Provides a convenient representation of the Vm Collections for range loops
     * @return A constant refrerence to the vector containing the Vm Collections
     */
    const std::vector<VmInstanceCollection *> &allVmCollections() { return virtualMachines; }

    /**
     * @brief FIXME: Figure out exactly what does this mean!
     * @return int The presumed numerical ID
     */
    int getNId() const { return nId; }

    /**
     * Gets the nth vm in the virtualmachine list of collections flattened.
     * @param index Index of the required vm.
     */
    VmInstance *getNthVm(int index);

    AppInstance *getAppInstance(int nIndex);

    // Managing the messages sent and received by the user.
    SM_UserVM *getRequestVmMsg() { return requestVmMsg; }
    SM_UserAPP *getRequestAppMsg() { return requestAppMsg; };
    SM_UserVM *getSubscribeVmMsg() { return subscribeVmMsg; };

    void setRequestApp(SM_UserAPP *requestAppMsg) { this->requestAppMsg = requestAppMsg; };
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

    /**
     * @brief Wheter the user did subscribe or not
     */
    bool hasSubscribed() const { return bSubscribe; }

    void setTimeoutMaxStart() { bTimeout_t2 = true; }
    void setTimeoutMaxRentTime() { bTimeout_t2 = true; }
    void setTimeoutMaxSubscribed() { bTimeout_t4 = true; }
    void setFinished(bool finished) { this->bFinished = finished; }
    void setSubscribe(bool bSubscribe) { this->bSubscribe = bSubscribe; }

    void addFinishedVMs(int newFinished) { numFinishedVMs += newFinished; }
    int getTotalVMs() const { return numTotalVMs; }

    /**
     * @brief If all of the rented vms ended their contract
     */
    bool allVmsFinished() const { return numTotalVMs <= numFinishedVMs; }

    // int getNumActiveSubscriptions() const { return numActiveSubscriptions; }
    // void setNumActiveSubscriptions(int numActiveSubscriptions) { this->numActiveSubscriptions = numActiveSubscriptions; }

    void startSubscription();
    void endSubscription();
};

#endif /* USERINSTANCE_H_ */
