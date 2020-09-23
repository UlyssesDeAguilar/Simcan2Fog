#include "NodeUserInfo_.h"

NodeUserInfo_delete::NodeUserInfo_delete(){

    userName = "";
    maxStartTime_t1 = 0;
    nRentTime_t2 = 0;
    maxSubTime_t3 = 0;
    maxSubscriptionTime_t4 = 0;
    appRequestMsg = nullptr;
}

NodeUserInfo_delete::~NodeUserInfo_delete() {

}

const std::string& NodeUserInfo_delete::getUserName() const {
    return userName;
}

void NodeUserInfo_delete::setUserName(const std::string& userName) {
    this->userName = userName;
}

const std::string& NodeUserInfo_delete::getVmId() const {
    return vmId;
}

void NodeUserInfo_delete::setVmId(const std::string& vmId) {
    this->vmId = vmId;
}

cMessage* NodeUserInfo_delete::getAppRequestMsg() const {
    return appRequestMsg;
}

void NodeUserInfo_delete::setAppRequestMsg(cMessage* appRequestMsg) {
    this->appRequestMsg = appRequestMsg;
}

int NodeUserInfo_delete::getMaxStartTimeT1() const {
    return maxStartTime_t1;
}

void NodeUserInfo_delete::setMaxStartTimeT1(int maxStartTimeT1) {
    maxStartTime_t1 = maxStartTimeT1;
}

int NodeUserInfo_delete::getRentTimeT2() const {
    return nRentTime_t2;
}

void NodeUserInfo_delete::setRentTimeT2(int rentTimeT2) {
    nRentTime_t2 = rentTimeT2;
}

int NodeUserInfo_delete::getMaxSubTimeT3() const {
    return maxSubTime_t3;
}

void NodeUserInfo_delete::setMaxSubTimeT3(int maxSubTimeT3) {
    maxSubTime_t3 = maxSubTimeT3;
}

int NodeUserInfo_delete::getMaxSubscriptionTimeT4() const {
    return maxSubscriptionTime_t4;
}

void NodeUserInfo_delete::setMaxSubscriptionTimeT4(int maxSubscriptionTimeT4) {
    maxSubscriptionTime_t4 = maxSubscriptionTimeT4;
}






