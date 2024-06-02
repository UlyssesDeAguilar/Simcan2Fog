#ifndef __SIMCAN_2_0_USER_APP_BASE_H_
#define __SIMCAN_2_0_USER_APP_BASE_H_

#include "Core/cSIMCAN_Core.h"
//#include "Architecture/Network/Stack/NetworkIOEvent_m.h"
#include "Messages/Syscall_m.h"

/**
 * @class SimcanAPI SimcanAPI.h "SimcanAPI.h"
 *
 * Base class for the user applications. This class contains the operating system calls interface.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2016-07-01
 */
class UserAppBase : public cSIMCAN_Core
{

protected:
   enum State
   {
      RUN,
      WAIT
   };

   enum Event
   {
      EXEC_START
   };

   uint32_t pid;             //!< Process id
   uint32_t vmId;            //!< Virtual machine id
   uint32_t pc;              //!< Program counter
   State state;              //!< Application state
   simtime_t operationStart; //!< Timestamp of the starting of an operation
   cQueue incomingPackets;   //!< The incoming packets from the opened sockets

   double startDelay;        //!< Starting time delay
   double connectionDelay;   //!< Connection delay time
   unsigned int myRank;      //!< Rank of the application's process
   string testID;            //!< Test ID
   string appInstance;       //!< Name of the application's instance
   string vmInstance;        //!< Name of the vm's instance
   string userInstance;      //!< Name of the user's instance
   bool isDistributedApp;    //!< Is a distributed application?
   bool debugUserAppBase;    //!< Show log messages of UserAppBase (for deep-debugging purpose only)

   cMessage *event{}; //!< Message reserved for auto events
   cGate *inGate;   /**< Input gate from OS. */
   cGate *outGate;  /**< Output gate to OS. */

   
   struct AppSocket
   {
      hypervisor::ConnectionMode mode;
   };


   virtual ~UserAppBase();
   virtual void initialize() override;
   virtual void finish() override;
   virtual void processSelfMessage(cMessage *msg) override;
   virtual void processRequestMessage(SIMCAN_Message *msg) override;
   virtual void processResponseMessage(SIMCAN_Message *msg) override;
   virtual void sendRequestMessage(SIMCAN_Message *msg, cGate *outGate) override;
   virtual void scheduleExecStart();

   virtual void run() = 0;
   void execute(double MIs);
   void execute(simtime_t cpuTime);
   void read(double bytes);
   void write(double bytes);
   void open_client(int targetPort, hypervisor::ConnectionMode mode);
   void open_server(int targetPort, hypervisor::ConnectionMode mode, const char *serviceName = nullptr);
   void resolve(const char *domainName);
   void _send(int socketFd, SIMCAN_Message *msg, uint32_t targetPort = 0, uint32_t targetIp = 0);
   void recv(int socketFd);
   void close(int socketFd);
   void abort();
   void _exit();

public:
   class ICallback
   {
   public:
      virtual void returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) {};
      virtual void returnRead(simtime_t timeElapsed) {};
      virtual void returnWrite(simtime_t timeElapsed) {};
      virtual void returnResolve(simtime_t timeElapsed) {};
   };

   void setReturnCallback(ICallback *callback) { this->callback = callback; }

private:
   ICallback *callback;
   virtual cGate *getOutGate(cMessage *msg) override;
};

#endif
