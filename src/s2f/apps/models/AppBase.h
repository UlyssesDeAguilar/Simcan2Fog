#ifndef __SIMCAN_2_0_USER_APP_BASE_H_
#define __SIMCAN_2_0_USER_APP_BASE_H_

#include "s2f/architecture/net/stack/resolver/DnsResolver.h"
// #include "s2f/architecture/Network/Stack/StackServiceType.h"
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
class AppBase : public cSIMCAN_Core, public TcpSocket::ICallback, public UdpSocket::ICallback, public DnsResolver::ResolverCallback
{
protected:
   using SocketQueue = std::map<int, cQueue>;

   enum Event
   {
      EXEC_START
   };

   uint32_t pid;             //!< Process id
   uint32_t vmId;            //!< Virtual machine id
   simtime_t operationStart; //!< Timestamp of the starting of an operation

   DnsResolver *resolver;
   SocketMap socketMap;
   SocketQueue socketQueue;

   const char *nsPath{};
   const char *parentPath{};
   double startDelay;      //!< Starting time delay
   double connectionDelay; //!< Connection delay time
   unsigned int myRank;    //!< Rank of the application's process
   string testID;          //!< Test ID
   string appInstance;     //!< Name of the application's instance
   string vmInstance;      //!< Name of the vm's instance
   string userInstance;    //!< Name of the user's instance
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
   int open(uint16_t localPort, ConnectionMode mode);
   void connect(int socketFd, uint32_t destIp, uint16_t destPort);
   // void openService(int targetPort, const char *serviceName = nullptr);

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
      virtual void handleResolverReturned(const L3Address ip, bool resolved) = 0;
      virtual void handleDataArrived(int sockFd, Packet *p) = 0;
      virtual void handleConnectReturn(int sockFd, bool connected) = 0;
      virtual bool handlePeerClosed(int sockFd) = 0;
   };

   // UDP/SOCK_DGRAM
   virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
   virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
   virtual void socketClosed(UdpSocket *socket) override;

   // TCP/SOCK_STREAM
   virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
   virtual void socketAvailable(TcpSocket *socket, TcpAvailableInfo *availableInfo) override { error("Accepting connections is not supported"); }
   virtual void socketEstablished(TcpSocket *socket) override;
   virtual void socketPeerClosed(TcpSocket *socket) override;
   virtual void socketClosed(TcpSocket *socket) override;
   virtual void socketFailure(TcpSocket *socket, int code) override;
   virtual void socketStatusArrived(TcpSocket *socket, TcpStatusInfo *status) override {}
   virtual void socketDeleted(TcpSocket *socket) override {}

   // DNS RESOLVER
   virtual void handleResolverReturned(const L3Address& address, bool resolved) override { Enter_Method_Silent(); callback->handleResolverReturned(address, resolved); }
   
   void setReturnCallback(ICallback *callback) { this->callback = callback; }
private:
   ICallback *callback{};
   virtual cGate *getOutGate(cMessage *msg) override;
};

#endif
