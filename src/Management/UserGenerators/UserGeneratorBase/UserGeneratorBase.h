#ifndef __SIMCAN_2_0_USERGENERATOR_BASE_H_
#define __SIMCAN_2_0_USERGENERATOR_BASE_H_

#include "Management/CloudManagerBase/CloudManagerBase.h"
#include "Management/dataClasses/Users/CloudUserInstance.h"
#include "Management/dataClasses/Users/CloudUserInstanceTrace.h"
#include "Core/include/GroupVector.hpp"
#include "Messages/SM_UserVM_m.h"

#include <fstream>

/**
 * @brief Base class for User generators.
 * @details class parses and manages cloud users
 * @author Pablo Cerro Cañizares
 */
class UserGeneratorBase : public CloudManagerBase
{

protected:
    typedef std::map<std::string, CloudUserInstance *> UserMap;
    typedef std::vector<CloudUserInstance *> UserInstanceList;

    // group_vector<CloudUserInstance *> groupOfUsers; // Vector that contains the user instances (can be used in a flatenned version)
    std::vector<std::vector<CloudUserInstance *>> groupOfUsers; // Vector that contains the user instances
    UserInstanceList userInstances; // Vector that contains all the cloud (individual) user instances
    UserMap userHashMap;            // Hashmap to accelerate the management of the users (EDIT: It is an RB tree actually)

    int nUserInstancesFinished; // ??
    int nUserIndex;             // Index of the next user which must be processed

    cGate *fromCloudProviderGate; // Input gate from DataCentre
    cGate *toCloudProviderGate;   // Output gates to DataCentre

    /* Flags */
    bool showUserInstances;    // Show parsed user instances
    bool intervalBetweenUsers; // Flag for generating all the user instances at once
    bool shuffleUsers;         // Flag for suffling users
    bool activeCycles;         // ??

    InstanceRequestTimes timeoutsTemplate; // The "maximum" times acceptable by users
    string strUserTraceFilePath;

    int userTraceMaxVms;

    double startDelay;  // Starting time delay
    cPar *distribution; // Interval gap between users arrivals

    int numberOfCycles;
    bool isolateCycle;
    double durationOfCycle;
    cPar *cycleDistribution;

    int traceStartTime;
    int traceEndTime;

    // TODO: Delete
    SimTime m_dInitSim;

    /**
     * Destructor
     */
    ~UserGeneratorBase();

    /**
     * Initialize method. Invokes the parsing process to allocate the existing cloud users in the corresponding data structures.
     */
    virtual void initialize() override;
    virtual void initializeRequestHandlers() {};

    /**
     * Get the out Gate to the module that sent <b>msg</b>.
     *
     * @param msg Arrived message.
     * @return Gate (out) to module that sent <b>msg</b> or \a nullptr if gate not found.
     */
    virtual cGate *getOutGate(cMessage *msg) override;

    /**
     * Generates the collection of user instances that were defined in the initial configuration.
     *
     * This process is performed before the simulation starts (in cold).
     *
     */
    virtual void generateUsersBeforeSimulationStarts();

    /**
     * Shuffles the arrival time of the generated users
     */
    virtual void generateShuffledUsers();

    virtual void generateUserArrivalTimes();

    /**
     * Parses the current content of the users vector into a string.
     *
     * @return String containing the current status of the users vector.
     */
    virtual string usersIstancesToString();

    virtual CloudUserInstance *createCloudUserInstance(const CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances);

    void parseTraceFile();

    /**
     * Builds the VM request which corresponds with the provided user instance.
     */
    virtual SM_UserVM *createVmRequest(CloudUserInstance *pUserInstance);

    /**
     * Returns the time the next user will arrive to the cloud
     *
     * @param pUserInstance Pointer to the user instance.
     * @param last Last user arrival time.
     */
    virtual SimTime getNextTime(CloudUserInstance *pUserInstance, SimTime last);

    virtual SM_UserVM *createVmMessage();

    virtual CloudUserInstance *createNewUserFromTrace(Job_t jobIn, int totalUserInstance, int nCurrentNumber, int nUserInstance, int nTotalInstances);

    virtual CloudUser *createUserTraceType();
};

#endif
