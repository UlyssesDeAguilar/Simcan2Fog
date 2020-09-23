#include "NodeResourceRequest.h"

NodeResourceRequest::NodeResourceRequest(){

    nStartTime = nPrice = 0;

}

NodeResourceRequest::~NodeResourceRequest() {

}

cMessage* NodeResourceRequest::getAppRequestMsg() const {
    return appRequestMsg;
}

void NodeResourceRequest::setAppRequestMsg(cMessage* appRequestMsg) {
    this->appRequestMsg = appRequestMsg;
}
const std::string& NodeResourceRequest::getIp() const {
    return ip;
}

void NodeResourceRequest::setIp(const std::string& ip) {
    this->ip = ip;
}

int NodeResourceRequest::getMaxStartTimeT1() const {
    return maxStartTime_t1;
}

void NodeResourceRequest::setMaxStartTimeT1(int maxStartTimeT1) {
    maxStartTime_t1 = maxStartTimeT1;
}

int NodeResourceRequest::getMaxSubscriptionTimeT4() const {
    return maxSubscriptionTime_t4;
}

void NodeResourceRequest::setMaxSubscriptionTimeT4(int maxSubscriptionTimeT4) {
    maxSubscriptionTime_t4 = maxSubscriptionTimeT4;
}

int NodeResourceRequest::getMaxSubTimeT3() const {
    return maxSubTime_t3;
}

void NodeResourceRequest::setMaxSubTimeT3(int maxSubTimeT3) {
    maxSubTime_t3 = maxSubTimeT3;
}

int NodeResourceRequest::getRentTimeT2() const {
    return nRentTime_t2;
}

void NodeResourceRequest::setRentTimeT2(int rentTimeT2) {
    nRentTime_t2 = rentTimeT2;
}
unsigned int NodeResourceRequest::getTotalCpUs() const {
    return totalCPUs;
}

void NodeResourceRequest::setTotalCpUs(unsigned int totalCpUs) {
    totalCPUs = totalCpUs;
}

const std::string& NodeResourceRequest::getUserName() const {
    return userName;
}

void NodeResourceRequest::setUserName(const std::string& userName) {
    this->userName = userName;
}

unsigned int NodeResourceRequest::getTotalMemory() const {
    return nTotalMemory;
}

void NodeResourceRequest::setTotalMemory(unsigned int totalMemory) {
    nTotalMemory = totalMemory;
}


