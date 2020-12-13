#ifndef NODERESOURCEUSER_H_
#define NODERESOURCEUSER_H_

#include <string>
#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

class NodeResourceRequest{

    private:
        /** Name of the user*/
        std::string userName;

        /** Type of the node*/
        std::string nodeType;

        /** Identifier of the VM*/
        std::string strVmId;

        //Data related with the user request
        /** Number of CPUs requested*/
        unsigned int totalCPUs;

        /** Total memory requested*/
        double nTotalMemory;

        /** CPU speed requested*/
        unsigned int cpuSpeed;

        /** Total storage requested*/
        double totalDiskGB;

        /** Total memory requested requested*/
        double totalMemoryGB;

        /** The requested VM is used to */
        bool storage;

        //Times
        /** T1: Maximum time to start the execution of the applications over the VM*/
        int maxStartTime_t1;

        /** T2: Renting time*/
        int nRentTime_t2;

        /** T3: ?*/
        int maxSubTime_t3;

        /** T3: Maximum subscription time until timeout*/
        int maxSubscriptionTime_t4;

        /** Message that contains the application request*/
        cMessage *appRequestMsg;

        //Data provided by the DataCenter
        /** The execution start time, provided by the datacenter*/
        int nStartTime;

        /** Price of the execution*/
        int nPrice;

        /** Ip of the node where the application will be executed*/
        std::string ip;

    public:
        NodeResourceRequest();
        ~NodeResourceRequest();
        cMessage* getAppRequestMsg() const;
        void setAppRequestMsg(cMessage* appRequestMsg);

        //Getters-setters
        const std::string& getIp() const;
        void setIp(const std::string& ip);
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
        unsigned int getTotalCpUs() const;
        void setTotalCpUs(unsigned int totalCpUs);
        const std::string& getUserName() const;
        void setUserName(const std::string& userName);
        double getTotalMemory() const;
        void setTotalMemory(double totalMemory);
        int getPrice() const {return nPrice;}
        void setPrice(int price) {nPrice = price;}
        int getStartTime() const {return nStartTime;}
        void setStartTime(int startTime) {nStartTime = startTime;}
        const std::string& getNodeType() const {return nodeType;}
        void setNodeType(const std::string& nodeType) {this->nodeType = nodeType;}
        const std::string& getVmId() const {return strVmId;}
        void setVmId(const std::string& strVmId) {this->strVmId = strVmId;}
};

#endif /* NODEUSER_H_ */
