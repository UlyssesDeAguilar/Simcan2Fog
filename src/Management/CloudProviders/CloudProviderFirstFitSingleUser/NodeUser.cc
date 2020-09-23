#include "NodeUser.h"

NodeUser::NodeUser(){

}

NodeUser::~NodeUser() {

}


cMessage* NodeUser::getAppRequestMsg() const {
    return appRequestMsg;
}

void NodeUser::setAppRequestMsg(cMessage* appRequestMsg) {
    this->appRequestMsg = appRequestMsg;
}
const std::string& NodeUser::getIp() const {
    return ip;
}

void NodeUser::setIp(const std::string& ip) {
    this->ip = ip;
}

int NodeUser::getMaxStartTimeT1() const {
    return maxStartTime_t1;
}

void NodeUser::setMaxStartTimeT1(int maxStartTimeT1) {
    maxStartTime_t1 = maxStartTimeT1;
}

int NodeUser::getMaxSubscriptionTimeT4() const {
    return maxSubscriptionTime_t4;
}

void NodeUser::setMaxSubscriptionTimeT4(int maxSubscriptionTimeT4) {
    maxSubscriptionTime_t4 = maxSubscriptionTimeT4;
}

int NodeUser::getMaxSubTimeT3() const {
    return maxSubTime_t3;
}

void NodeUser::setMaxSubTimeT3(int maxSubTimeT3) {
    maxSubTime_t3 = maxSubTimeT3;
}

int NodeUser::getRentTimeT2() const {
    return nRentTime_t2;
}

void NodeUser::setRentTimeT2(int rentTimeT2) {
    nRentTime_t2 = rentTimeT2;
}
unsigned int NodeUser::getTotalCpUs() const {
    return totalCPUs;
}

void NodeUser::setTotalCpUs(unsigned int totalCpUs) {
    totalCPUs = totalCpUs;
}

const std::string& NodeUser::getUserName() const {
    return userName;
}

void NodeUser::setUserName(const std::string& userName) {
    this->userName = userName;
}

unsigned int NodeUser::getTotalMemory() const {
    return nTotalMemory;
}

void NodeUser::setTotalMemory(unsigned int totalMemory) {
    nTotalMemory = totalMemory;
}


