#ifndef SIMCAN_EX_EDGE_USER_H__
#define SIMCAN_EX_EDGE_USER_H__

#include "inet/common/INETDefs.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/applications/tcpapp/TcpServerHostApp.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/socket/SocketMap.h"
#include "s2f/users/edge/EdgeUsersControl_m.h"
#include "s2f/messages/SM_UserVM.h"
#include "s2f/messages/SM_UserAPP.h"
#include "s2f/messages/SM_VmExtend_m.h"

using namespace omnetpp;

namespace s2f
{
    namespace users
    {
        class EdgeUser : public cSimpleModule
        {
        protected:
            std::vector<cModule *> managedSensors; //!< Array of managed sensors
            std::map<opp_string, Vm> managedVms;   //!< Map of managed VMs
            std::vector<App> localApps;            //!< Array of local apps
            const char *deploymentZone;            //!< Preferred deployment zone
            GateInfo serialGates;                  //!< Serial gates information
            opp_string userId;                     //!< Unique user identifier

            // Kernel lifecycle

            virtual void initialize() override;
            virtual void handleMessage(cMessage *msg) override;
            virtual void finish() override;

            /**
             * @brief Initial entry point for the users action
             * @details This method is called at the startupTime of the model
             */
            virtual void startup();

            /**
             * @brief Handles a VM deployment update
             * 
             * @param update The response to the deployment
             */
            virtual void handleVmUpdate(SM_UserVM *update);

            /**
             * @brief Handles an app deployment update
             * 
             * @param update The response to the deployment
             */
            virtual void handleAppEvent(SM_UserAPP *update);

            /**
             * @brief Processes a serial event from the local device (PC/Controller)
             * 
             * @param msg The generated event
             */
            virtual void processSerialEvent(cMessage *msg);

            /**
             * @brief Turns all managed sensors on
             * 
             */
            virtual void turnSensorsOn();

            /**
             * @brief Turns all managed sensors off
             *
             */
            virtual void turnSensorsOff();
            
            /**
             * @brief Find a VM that acts as a platform by name
             * 
             * @param name Name of the VM to find
             * @return Pointer to the VM definition or nullptr if not found
             */
            virtual users::Vm *findPlatform(const char *name);

            /**
             * @brief Generates a unique identifier
             * 
             * @param name A name for the object
             * @return opp_string The constructed id
             */
            opp_string generateUniqueId(const char *name);

            /**
             * @brief Notifies an local app that a service which it depends upon it's up
             * 
             * @param localApp Name of the local application
             * @param service The service definition
             * @param domain The domain of the service
             */
            void sendServiceIsUp(const char *localApp, const App &service, const char *domain);
        };
    }
}

#endif /* SIMCAN_EX_EDGE_USER_H__ */
