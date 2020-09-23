#ifndef NODEUSER_H_
#define NODEUSER_H_

#include <string>
#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

class NodeUser{

    private:
        std::string userName;
        std::string nodeType;
        std::string strVmId;

        //Data related with the user request
        unsigned int totalCPUs;
        unsigned int nTotalMemory;
        unsigned int nAvailableCPUs;

        unsigned int cpuSpeed;
        double totalDiskGB;
        double totalMemoryGB;
        bool storage;

        bool bReserved;

        //Times
        int maxStartTime_t1;
        int nRentTime_t2;
        int maxSubTime_t3;
        int maxSubscriptionTime_t4;
        cMessage *appRequestMsg;

        //Data provided by the DataCenter
        //TODO: Proporcionar un array con las distintas propuestas
        int nStartTime;
        int nPrice;
        std::string ip;

    public:
        NodeUser();
        ~NodeUser();
        cMessage* getAppRequestMsg() const;
        void setAppRequestMsg(cMessage* appRequestMsg);

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

        unsigned int getTotalCpUs() const;
        void setTotalCpUs(unsigned int totalCpUs);
        const std::string& getUserName() const;
        void setUserName(const std::string& userName);

        unsigned int getTotalMemory() const;
        void setTotalMemory(unsigned int totalMemory);

    int getPrice() const {
        return nPrice;
    }

    void setPrice(int price) {
        nPrice = price;
    }

    int getStartTime() const {
        return nStartTime;
    }

    void setStartTime(int startTime) {
        nStartTime = startTime;
    }

    const std::string& getNodeType() const {
        return nodeType;
    }

    void setNodeType(const std::string& nodeType) {
        this->nodeType = nodeType;
    }

    const std::string& getStrVmId() const {
        return strVmId;
    }

    void setStrVmId(const std::string& strVmId) {
        this->strVmId = strVmId;
    }

    unsigned int getAvailableCpUs() const {
        return nAvailableCPUs;
    }

    void setAvailableAllCpus() {
        nAvailableCPUs = totalCPUs;
    }

    void setAvailableCpUs(unsigned int availableCpUs) {
        nAvailableCPUs = availableCpUs;
    }

    bool isReserved() const {
        return bReserved;
    }

    void setReserved(bool reserved) {
        bReserved = reserved;
    }
};

#endif /* NODEUSER_H_ */
