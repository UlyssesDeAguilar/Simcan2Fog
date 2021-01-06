#ifndef __SIMCAN_2_0_CLOUDMANAGER_BASE_H_
#define __SIMCAN_2_0_CLOUDMANAGER_BASE_H_

#include "Core/cSIMCAN_Core.h"
#include "Management/parser/UserPriorityListParser.h"
#include "Management/parser/SlaListParser.h"
#include "Management/parser/VmListParser.h"
#include "Management/parser/AppListParser.h"
#include "Management/utils/LogUtils.h"
#include "Messages/SM_CloudProvider_Control_m.h"
#include <algorithm>
#include <functional>


#define INITIAL_STAGE  "INITIAL_STAGE"
#define EXEC_APP_END  "EXEC_APP_END"
#define EXEC_VM_RENT_TIMEOUT "EXEC_VM_RENT_TIMEOUT"
#define EXEC_APP_END_SINGLE "EXEC_APP_END_SINGLE"
#define EXEC_APP_END_SINGLE_TIMEOUT "EXEC_APP_END_SINGLE_TIMEOUT"
#define MANAGE_SUBSCRIBTIONS  "MANAGE_SUBSCRIBTIONS"
#define USER_SUBSCRIPTION_TIMEOUT  "SUBSCRIPTION_TIMEOUT"
#define SIMCAN_MESSAGE "SIMCAN_Message"
#define CPU_STATUS "CPU_STATUS"

/**
 * Base class for Cloud Managers.
 *
 * This class parses and manages VMs and users
 */
class CloudManagerBase: public cSIMCAN_Core{

    protected:
        /** Show parsed slas */
        bool showSlas;

        /** Show parsed users and VMs */
        bool showUsersVms;

        /** Shows the parsed applications */
        bool showApps;

        /** Flag that indicates if the process has been finished*/
        bool bFinished;

        /** Vector that contains the set of application types used in the current simulation */
        std::vector<Application*> appTypes;

        /** Vector that contains the types of VM used in the current simulation */
        std::vector<VirtualMachine*> vmTypes;

        /** Vector that contains the types of users generated in the current simulation */
        std::vector<CloudUser*> userTypes;

        /** Handler maps */
        std::map<std::string, std::function<void(cMessage*)>> selfHandlers;
        std::map<int, std::function<void(SIMCAN_Message*)>> requestHandlers;
        std::map<int, std::function<void(SIMCAN_Message*)>> responseHandlers;

        /**
         * Destructor
         */
        ~CloudManagerBase();

        /**
         * Initialize method. Invokes the parsing process to allocate the existing VMs and users in the corresponding data structures.
         */
        virtual void initialize();

        virtual void initializeSelfHandlers(){};
        virtual void initializeRequestHandlers(){};
        virtual void initializeResponseHandlers(){};

        /**
         * Parses the config of the simulation.
         */
        virtual void parseConfig();

        /**
         * Parses each application type used in the simulation.
         * These applications are allocated in the <b>appTypes</b> vector.
         *
         * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
         */
        virtual int parseAppList();

        /**
         * Parses each VM type used in the simulation. These VMs are allocated in the <b>vmTypes</b> vector.
         *
         * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
         */
        virtual int parseVmsList();

        /**
         * Parses each user type used in the simulation. These users are allocated in the <b>userTypes</b> vector.
         *
         * @return If the parsing process is successfully executed, this method returns SC_OK. In other case, it returns SC_ERROR.
         */
        virtual int parseUsersList();

       /**
        * Converts the parsed applications to string format. Usually, this method is invoked for debugging/logging purposes.
        *
        * @return String containing the parsed applications.
        */
        std::string appsToString();

        /**
         * Converts the parsed VMs into string format.
         *
         * @return A string containing the parsed VMs.
         */
        std::string vmsToString();

        /**
         * Converts the parsed user into string format.
         *
         * @return A string containing the parsed users.
         */
        std::string usersToString();

        /**
         * Search for the application <b>appName</b>.
         *
         * @param appName Instance name of the requested application.
         * @return If the requested application is located in the vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
         */
         Application* findApplication (std::string appName);

        /**
         * Search for a specific type of VM.
         *
         * @param vmType Type of a VM.
         * @return If the requested type of VM is located in the vmTypes vector, then a pointer to its object is returned. In other case, a \a nullptr is returned.
         */
         VirtualMachine* findVirtualMachine (std::string vmType);


       /**
        * Search for a specific type of CloudUser.
        *
        * @param userType Type of a user.
        * @return If the requested type of user is located in the userTypes vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
        */
        CloudUser* findUser (std::string userType);

       /**
        * Get the out Gate to the module that sent <b>msg</b>.
        *
        * @param msg Arrived message.
        * @return Gate (out) to module that sent <b>msg</b> or \a nullptr if gate not found.
        */
        virtual cGate* getOutGate (cMessage *msg) = 0;

       /**
        * Process a self message.
        *
        * @param msg Received (self) message.
        */
        virtual void processSelfMessage (cMessage *msg) override;

       /**
        * Process a request message.
        *
        * @param sm Incoming message.
        */
        virtual void processRequestMessage (SIMCAN_Message *sm) override;

       /**
        * Process a response message from an external module.
        *
        * @param sm Incoming message.
        */
        virtual void processResponseMessage (SIMCAN_Message *sm) override;
};

#endif
