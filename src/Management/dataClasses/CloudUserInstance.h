#ifndef CLOUDUSERINSTANCE_H_
#define CLOUDUSERINSTANCE_H_

#include "UserInstance.h"
#include "VmInstanceCollection.h"
#include "CloudUser.h"

#include "Messages/SM_UserVM.h"
#include "Messages/SM_UserAPP.h"

/**
 * Class that represent a modeled user in SIMCAN for cloud environments.
 *
 * Basically, the instance of one user contains:
 *  - A collection of applications to be executed.
 *  - A collection of VMs where these applications must be executed.
 */
class CloudUserInstance: public UserInstance {

    private:

        /**
         * Vector of Virtual Machines configured for this user.
         */
        std::vector<VmInstanceCollection*> virtualMachines;
        std::vector<AppInstance*> appInstances;

        SM_UserVM* requestVmMsg;
        SM_UserAPP* requestAppMsg;
        SM_UserVM* subscribeVmMsg;

        //Request times.
        int maxStartTime_t1;
        int nRentTime_t2;
        int maxSubTime_t3;
        int maxSubscriptionTime_t4;
        int nId;

        int numFinishedVMs;
        int numTotalVMs;

        SimTime dArrival2Cloud;
        SimTime dInitTime;
        SimTime dInitExec;
        SimTime dEndTime;

        bool bSubscribe;
        bool bTimeout_t2;
        bool bTimeout_t4;
        bool bFinished;

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
        CloudUserInstance(CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances);

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
        virtual std::string toString (bool includeAppsParameters, bool includeVmFeatures);

        /**
         *  Return the number of virtual machines collections
         */
        int getNumberVmCollections(){return virtualMachines.size();};

        /**
         *  Return the number of application collections
         */
        int getNumberAppCollections(){return applications.size();};

        /**
         * Get an specific Vm collection
         * @param nCollection The ID of virtual machine collection to be retrieved.
         * @return Collection of VM instances.
         */
        VmInstanceCollection* getVmCollection(int nCollection);

        /**
         * Gets the number of instances of a specific App collection
         * @param nCollection The ID of the app collection to be retrieved.
         * @return Size of the collection of APP instances.
         */
        int getAppCollectionSize(int nIndex);


        AppInstance* getAppInstance(int nIndex);

        //Managing the messages sent and received by the user.
        SM_UserVM* getRequestVmMsg(){return requestVmMsg;}
        SM_UserAPP* getRequestAppMsg(){return requestAppMsg;};
        SM_UserVM* getSubscribeVmMsg(){return subscribeVmMsg;};

        void setRequestApp(SM_UserAPP* requestAppMsg){this->requestAppMsg = requestAppMsg;};
        void setRequestVmMsg(SM_UserVM* requestVmMsg){this->requestVmMsg=requestVmMsg;}

        //Getters for the T1, ..., T4
        int getT1(){return maxStartTime_t1;};
        int getT2(){return nRentTime_t2;};
        int getT3(){return maxSubTime_t3;};
        int getT4(){return maxSubscriptionTime_t4;};

        void setRentTimes(int maxStartTime_t1, int nRentTime_t2, int maxSubTime_t3, int maxSubscriptionTime_t4);

        SimTime getEndTime() const;
        SimTime getInitTime() const;
        SimTime getArrival2Cloud() const;
        SimTime getInitExecTime()  const;

        bool isFinished() const;
        bool isTimeout();
        bool isTimeoutMaxRent();
        bool isTimeoutSubscribed();
        bool hasSubscribed();
        bool allVmsFinished();

        void setArrival2Cloud(SimTime arrival2Cloud);
        void setTimeoutMaxStart();
        void addFinishedVMs(int newFinished);
        void setTimeoutMaxRentTime();
        void setTimeoutMaxSubscribed();
        void setFinished(bool finished);
        void setEndTime(SimTime endTime);
        void setInitTime(SimTime initTime);
        void setSubscribe(bool bSubscribe);
        void setInitExecTime(SimTime dExec);
        int getId() const;
        int getTotalVMs() const;
        void setId(int id);

        /**
         * Gets the nth vm in the virtualmachine list of collections flattened.
         * @param index Index of the required vm.
         */
        VmInstance* getNthVm(int index);

    protected:

       /**
        * Creates the corresponding VMs instances to be included in the <b>virtualMachines</b> vector.
        *
        * @param vmPtr Pointer to the </b>VirtualMachine</b> object that contains the main features of the generated VM.
        * @param numInstances Number of instances of the generated VM.
        */
       void insertNewVirtualMachineInstances (VirtualMachine* vmPtr, int numInstances, int nRentTime, int total, int offset);

       /**
        *Process the application collection in order to include them in a single vector
        */
       void processApplicationCollection();

       int getNumVms(std::string strType, CloudUser *ptrUser);
};

#endif /* USERINSTANCE_H_ */
