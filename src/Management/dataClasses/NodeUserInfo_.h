#ifndef NODEUSERINFO_H_
#define NODEUSERINFO_H_

#include <string>
#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

class NodeUserInfo_delete{

    //TODO: Comentar y cambiar el nombre!
    private:
        std::string userName;
        std::string vmId;
        int maxStartTime_t1;
        int nRentTime_t2;
        int maxSubTime_t3;
        int maxSubscriptionTime_t4;
        cMessage *appRequestMsg;

    public:
        NodeUserInfo_delete();
        ~NodeUserInfo_delete();

        const std::string& getUserName() const;
        void setUserName(const std::string& userName);
        const std::string& getVmId() const;
        void setVmId(const std::string& vmId);
        cMessage* getAppRequestMsg() const;
        void setAppRequestMsg(cMessage* appRequestMsg);

        int getMaxStartTimeT1() const;
        void setMaxStartTimeT1(int maxStartTimeT1);
        int getRentTimeT2() const;
        void setRentTimeT2(int rentTimeT2);
        int getMaxSubTimeT3() const;
        void setMaxSubTimeT3(int maxSubTimeT3);
        int getMaxSubscriptionTimeT4() const;
        void setMaxSubscriptionTimeT4(int maxSubscriptionTimeT4);

};

#endif /* NODEUSERINFO_H_ */
