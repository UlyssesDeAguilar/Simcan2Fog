#ifndef NODERESOURCEUSER_H_
#define NODERESOURCEUSER_H_

#include <string>
#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

class NodeResourceRequest
{
private:
    std::string userName; /** Name of the user*/
    std::string nodeType; /** Type of the node*/
    std::string strVmId;  /** Identifier of the VM*/

    // Data related with the user request
    unsigned int totalCPUs; /** Number of CPUs requested*/
    double nTotalMemory;    /** Total memory requested*/
    unsigned int cpuSpeed;  /** CPU speed requested*/
    double totalDiskGB;     /** Total storage requested*/
    double totalMemoryGB;   /** Total memory requested*/
    bool storage;           /** The requested VM is used to */

    // Times
    int maxStartTime_t1;        /** T1: Maximum time to start the execution of the applications over the VM*/
    int nRentTime_t2;           /** T2: Renting time*/
    int maxSubTime_t3;          /** T3: ?? => Maximum time to be in queue? */
    int maxSubscriptionTime_t4; /** T3: Maximum subscription time until timeout*/

    /** Message that contains the application request*/
    cMessage *appRequestMsg;

    // Data provided by the DataCentre
    /** The execution start time, provided by the datacentre*/
    int nStartTime;

    /** Price of the execution*/
    int nPrice;

    /** Ip of the node where the application will be executed*/
    std::string ip;

public:
    NodeResourceRequest();
    ~NodeResourceRequest();
    cMessage *getAppRequestMsg() const;
    void setAppRequestMsg(cMessage *appRequestMsg);

    // Getters-setters
    const std::string &getIp() const;
    void setIp(const std::string &ip);
    int getMaxStartTimeT1() const;
    void setMaxStartTimeT1(int maxStartTimeT1);
    int getMaxSubscriptionTimeT4() const;
    void setMaxSubscriptionTimeT4(int maxSubscriptionTimeT4);
    int getMaxSubTimeT3() const;
    void setMaxSubTimeT3(int maxSubTimeT3);
    int getRentTimeT2() const;
    void setRentTimeT2(int rentTimeT2);
    bool isReserved() const;
    void setReserved(bool reserved);
    unsigned int getTotalCpus() const;
    void setTotalCpUs(unsigned int totalCpUs);
    const std::string &getUserName() const;
    void setUserName(const std::string &userName);
    double getTotalMemory() const;
    void setTotalMemory(double totalMemory);
    int getPrice() const { return nPrice; }
    void setPrice(int price) { nPrice = price; }
    int getStartTime() const { return nStartTime; }
    void setStartTime(int startTime) { nStartTime = startTime; }
    const std::string &getNodeType() const { return nodeType; }
    void setNodeType(const std::string &nodeType) { this->nodeType = nodeType; }
    const std::string &getVmId() const { return strVmId; }
    void setVmId(const std::string &strVmId) { this->strVmId = strVmId; }

    double getTotalDiskGb() const;
    void setTotalDiskGb(double totalDiskGb);
};

#endif /* NODEUSER_H_ */
