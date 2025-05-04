#ifndef __SIMCAN_2_0_USER_APP_BASE_H_
#define __SIMCAN_2_0_USER_APP_BASE_H_

#include "s2f/architecture/net/stack/resolver/StubDnsRequest_m.h"
#include "s2f/architecture/net/stack/proxy/ProxyServiceRequest_m.h"
#include "s2f/core/cSIMCAN_Core.h"
#include "s2f/os/hypervisors/common.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/common/socket/SocketMap.h"
#include "inet/networklayer/common/L3AddressResolver.h"

using namespace hypervisor;
using namespace inet;

/**
 * @class SimcanAPI SimcanAPI.h "SimcanAPI.h"
 *
 * Base class for the user applications. This class contains the operating system calls interface.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 2016-07-01
 */
class AppBase : public cSIMCAN_Core, public TcpSocket::ICallback, public UdpSocket::ICallback
{
protected:
   enum Event
   {
      EXEC_START,
      POLL,
      POLL_TIMEOUT,
      SEND_DELAYED
   };

   uint32_t pid;             //!< Process id
   uint32_t vmId;            //!< Virtual machine id
   simtime_t operationStart; //!< Timestamp of the starting of an operation

   cGate *resolverGate{};
   SocketMap socketMap;

   double startDelay;      //!< Starting time delay
   double connectionDelay; //!< Connection delay time
   unsigned int myRank;    //!< Rank of the application's process
   const char* testID;          //!< Test ID
   const char* appInstance;     //!< Name of the application's instance
   const char* vmInstance;      //!< Name of the vm's instance
   const char* userInstance;    //!< Name of the user's instance
   bool isDistributedApp;  //!< Is a distributed application?
   bool debugUserAppBase;  //!< Show log messages of UserAppBase (for deep-debugging purpose only)

   cMessage *event{}; //!< Message reserved for auto events
   cGate *inGate;     /**< Input gate from OS. */
   cGate *outGate;    /**< Output gate to OS. */
   int socketInId;    /**< Input gate to socket */
   int socketOutId;   /**< Output gate to socket */

   virtual void initialize() override;
   virtual void finish() override;
   virtual void handleMessage(cMessage *msg) override;
   void syscallFillData(Syscall *syscall, SyscallCode code);
   // virtual void processSelfMessage(cMessage *msg) override;
   virtual void processRequestMessage(SIMCAN_Message *msg) override { error("This module doesn't take requests"); }
   virtual void processResponseMessage(SIMCAN_Message *msg) override;
   virtual void sendRequestMessage(SIMCAN_Message *msg, cGate *outGate) override;
   virtual void scheduleExecStart();

   // The API
   void execute(double MIs);
   void execute(simtime_t cpuTime);
   void read(double bytes);
   void write(double bytes);
   void resolve(const char *domainName);
   int open(uint16_t localPort, ConnectionMode mode, const char* interfaceName = nullptr);
   void connect(int socketFd, const L3Address &destIp, const uint16_t &destPort);
   void listen (int socketFd);
   void registerService(const char *serviceName, int sockFd);
   void unregisterService(const char *serviceName, int sockFd);

   void _send(int socketFd, Packet *packet) { _send(socketFd, packet, 0, 0); }
   void _send(int socketFd, Packet *packet, uint32_t destIp, uint16_t destPort);
   // Packet *recv(int socketFd);

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
      virtual void handleResolutionFinished(const L3Address ip, bool resolved) = 0;
      virtual void handleDataArrived(int sockFd, Packet *p) = 0;
      virtual bool handleClientConnection(int sockFd, const L3Address &remoteIp, const uint16_t &remotePort) { return false; }
      virtual void handleConnectReturn(int sockFd, bool connected) = 0;
      virtual bool handlePeerClosed(int sockFd) = 0;
   };

   // UDP/SOCK_DGRAM
   virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
   virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
   virtual void socketClosed(UdpSocket *socket) override;

   // TCP/SOCK_STREAM
   virtual void socketDataArrived(TcpSocket *socket, Packet *packet, bool urgent) override;
   virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override;
   virtual void socketEstablished(TcpSocket *socket) override;
   virtual void socketPeerClosed(TcpSocket *socket) override;
   virtual void socketClosed(TcpSocket *socket) override;
   virtual void socketFailure(TcpSocket *socket, int code) override;
   virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {}
   virtual void socketDeleted(TcpSocket *socket) override {}
   void setReturnCallback(ICallback *callback) { this->callback = callback; }

private:
   ICallback *callback{};
   virtual cGate *getOutGate(cMessage *msg) override;
};

#endif
