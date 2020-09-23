#ifndef __SIMCAN_2_0_USER_APP_BASE_H_
#define __SIMCAN_2_0_USER_APP_BASE_H_

#include "Core/cSIMCAN_Core.h"
using std::vector;

/**
 * @class SimcanAPI SimcanAPI.h "SimcanAPI.h"
 *
 * Base class for the user applications. This class contains the operating system calls interface.
 * 
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2016-07-01
 */
class UserAppBase: public cSIMCAN_Core{


	protected:

        /**< Starting time delay */
        double startDelay;

        /**< Connection delay time */
        double connectionDelay;

        /**< Is a distributed application? */
        bool isDistributedApp;

        /**< Rank of the application's process */
        unsigned int myRank;

        /**< Test ID */
        string testID;

        /**< Name of the application's instance */
        string appInstance;

        /**< Show log messages of UserAppBase (for deep-debugging purpose only) */
        bool debugUserAppBase;

        /**< Input gate from OS. */
        cGate* inGate;

        /**< Output gate to OS. */
        cGate* outGate;



                           /**
                            * Structure that represents a connection between two TCP applications.
                            */
                            struct SIMCAN_App_Connector{
                                string localAddress;			/**< Local Address. */
                                string destAddress;				/**< Destination Address. */
                                int localPort;					/**< Local Port */
                                int destPort;					/**< Destination Port */
                                int id;							/**< Local connection ID */
                                int connectionId;				/**< Connection Id (for sockets) */
                            };
                            typedef struct SIMCAN_App_Connector connector;

                            /**< Local IP */
                            string appLocalIP;

                            /**< Local port */
                            int appLocalPort;

                           /** connector vector that contains the corresponding data to establish connection with servers.
                            * Note: Must be initialized on derived classes.
                            */
                            vector <connector> connections;


		
	   /**
		* Destructor
		*/
	    ~UserAppBase();


	   /**
		* Module initialization.
		*/
		virtual void initialize();			   

	   /**
		* Module ending.
		*/
		virtual void finish();
		

	   /**
		* Process a self message.
		* @param msg Self message.
		*/
		virtual void processSelfMessage (cMessage *msg) = 0;


	   /**
		* Process a request message.
		* @param sm Request message.
		*/
		virtual void processRequestMessage (SIMCAN_Message *sm) = 0;


	   /**
		* Process a response message from a module in the local node.
		* @param sm Response message.
		*/
		virtual void processResponseMessage (SIMCAN_Message *sm) = 0;
		
		

		// ----------------- CPU ----------------- //
		
		/**
		 * Request for CPU execution
		 * This function creates and sends the corresponding message to CPU module.
		 * @param MIs Million instructions to be executed.		 
		 */
		void SIMCAN_request_cpu (double MIs);
		
		
		/**
		 * Request for CPU execution
		 * This function creates and sends the corresponding message to CPU module.
		 * @param cpuTime Time to execute the current CPU request.		 
		 */
		void SIMCAN_request_cpuTime (simtime_t cpuTime);
		
		
		
	private:

	   /**
		* Get the out Gate to the module that sent <b>msg</b>.
		* @param msg Arrived message.
		* @return Gate (out) to module that sent <b>msg</b> or NULL if gate not found.
		*/
		cGate* getOutGate (cMessage *msg);		
		
};

#endif

