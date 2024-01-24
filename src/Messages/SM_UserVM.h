/*
 * SM_UserVM.h
 *
 *  Created on: Apr 6, 2017
 *      Author: pablo
 */

#ifndef SM_USERVM_H_
#define SM_USERVM_H_

#include "SM_UserVM_m.h"

class SM_UserVM : public SM_UserVM_Base
{
public:
     /**
      * Destructor.
      */
     virtual ~SM_UserVM();

     /**
      * Constructor.
      */
     SM_UserVM();
     SM_UserVM(const char *name, short kind);

     int addVmRequest(VM_Request vmReq);
     int createNewVmRequest(std::string strType, std::string instanceId, int maxStartTime_t1, int nRentTime_t2, int maxSubTime_t3, int maxSubscriptionTime_t4);
     int createNewVmSubscription(std::string strType, int nRentTime, int nMaxStartTime, int nMaxSubTime);
     int getTotalVmsRequests() const { return getVmsArraySize(); }
     std::string getVmRequestType(int nIndex);

     void printUserVM();
     void createResponse(int nIndex, bool bAccepted, int nTime, std::string strIp, int nPrice);
     bool hasResponse(int nIndexRq, int nIndexRes);
     VM_Response *getResponse(int nIndexRq, int nIndexRes);
     int getRentTime(int nIndex);
     int getMaxStartTime(int nIndex);

     int getMaxSubscribetime(int nIndex);
     void clearResponses();

     virtual SM_UserVM *dup() const;
     virtual SM_UserVM *dup(std::string strVmId) const;

     SM_UserVM_Finish *getTimeoutSubscribeMsg();
     void setTimeoutSubscribeMsg(SM_UserVM_Finish *pMsg);

private:
     SM_UserVM_Finish *pMsgTimeoutSub;
};

#endif /* SM_USERVM_H_ */
