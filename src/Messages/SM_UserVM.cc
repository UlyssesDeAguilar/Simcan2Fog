/*
 * SM_UserVM.cc
 *
 *  Created on: Apr 6, 2017
 *      Author: pablo
 */

#include "SM_UserVM.h"
#include "Management/utils/LogUtils.h"

SM_UserVM::SM_UserVM() {
    // TODO Auto-generated constructor stub
    this->pMsgTimeoutSub = nullptr;
}

SM_UserVM::~SM_UserVM() {
    // TODO Auto-generated destructor stub
}
SM_UserVM::SM_UserVM(const char *name, short kind) : ::SM_UserVM_Base(name,kind)
{
    this->pMsgTimeoutSub = nullptr;
}

SM_UserVM* SM_UserVM::dup() const
{
    SM_UserVM* pRet;
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Starting full dup" << endl;
    pRet = new SM_UserVM();

    pRet->setDEndSubscriptionTime(getDEndSubscriptionTime());
    pRet->setDStartSubscriptionTime(getDStartSubscriptionTime());
    pRet->setUserID(getUserID());
    pRet->setStrVmId(getStrVmId());

    for(int i = 0; i < this->getVmsArraySize(); i++)
      {
        VM_Request vmReq = getVms(i);
        pRet->createNewVmRequest(vmReq.strVmType, vmReq.strVmId, vmReq.maxStartTime_t1, vmReq.nRentTime_t2, vmReq.maxSubTime_t3, vmReq.maxSubscriptionTime_t4);
        for(int j = 0; j < vmReq.responseList.size(); j++)
        {
            VM_Response vmRes = vmReq.responseList.at(j);
            pRet->createResponse(i,vmRes.nOperationResult==1,vmRes.startTime, vmRes.strIp,vmRes.nPrice);
        }
    }

    //TODO: pMsgTimeoutSub pasando de momento

    // Reserve memory to trace!
    pRet->setTraceArraySize (getTraceArraySize());

    // Copy trace!
    for (int i=0; i<trace.size(); i++){
        pRet->addNodeTrace (trace[i].first, trace[i].second);
    }

    return pRet;
}

SM_UserVM* SM_UserVM::dup(std::string strVmId) const
{
    SM_UserVM* pRet;
    bool found;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Starting partial dup" << endl;
    found = false;
    pRet = new SM_UserVM();

    pRet->setDEndSubscriptionTime(getDEndSubscriptionTime());
    pRet->setDStartSubscriptionTime(getDStartSubscriptionTime());
    pRet->setUserID(getUserID());
    pRet->setStrVmId(strVmId.c_str());

    for(int i=0;i<this->getVmsArraySize();i++)
      {
        VM_Request vmReq = getVms(i);
        if (vmReq.strVmId.compare(strVmId) == 0)
          {
            found = true;
            pRet->createNewVmRequest(vmReq.strVmType, vmReq.strVmId, vmReq.maxStartTime_t1, vmReq.nRentTime_t2, vmReq.maxSubTime_t3, vmReq.maxSubscriptionTime_t4);
            for(int j = 0; j < vmReq.responseList.size(); j++)
              {
                VM_Response vmRes = vmReq.responseList.at(j);
                pRet->createResponse(i,vmRes.nOperationResult==1,vmRes.startTime, vmRes.strIp,vmRes.nPrice);
              }

            break;
          }
      }

    if (found)
      {
        //TODO: pMsgTimeoutSub pasando de momento

        // Reserve memory to trace!
        pRet->setTraceArraySize (getTraceArraySize());

        // Copy trace!
        for (int i=0; i<trace.size(); i++)
            pRet->addNodeTrace (trace[i].first, trace[i].second);
      }
    else
      {
        delete (pRet);
        pRet = nullptr;
      }

    return pRet;
}

int SM_UserVM::createNewVmRequest(std::string strType, std::string instanceId, int maxStartTime_t1, int nRentTime_t2, int maxSubTime_t3, int maxSubscriptionTime_t4)
{
    VM_Request vmReq;


    //Fill
    vmReq.strVmId = instanceId;
    vmReq.strVmType=strType;
    vmReq.maxStartTime_t1 = maxStartTime_t1;
    vmReq.nRentTime_t2 = nRentTime_t2;
    vmReq.maxSubTime_t3 = maxSubTime_t3;
    vmReq.maxSubscriptionTime_t4 = maxSubscriptionTime_t4;

    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - VmId: " << instanceId << " | maxStartTime_t1: "<< maxStartTime_t1 <<
            " | rentTime_t2: " << nRentTime_t2 <<" | maxSubTime: " <<maxSubTime_t3<< " | MaxSubscriptionTime_T4:" << maxSubscriptionTime_t4<< endl;

    return addVmRequest(vmReq);
}
int SM_UserVM::addVmRequest(VM_Request vmReq)
{
    int nVmId, nTotalSize;

    nVmId=0;

    //Resize the array size, and return the id of the request
    nVmId = getVmsArraySize();
    nTotalSize = nVmId+1;
    setVmsArraySize(nTotalSize);

    //Insert the request
    setVms(nVmId, vmReq);

    return nVmId;
}
void SM_UserVM::printUserVM()
{
    int nRequests, nResponses;
    VM_Request vmReq;
    VM_Response vmRes;

    //Print Message
    EV_INFO << "SM_UserVM::printUserVM - Init" << endl;
    nRequests = getVmsArraySize();
    EV_INFO << "User received: " << getUserID() << endl;
    EV_INFO << "Total Elements: "  << nRequests <<endl;

    for(int i=0;i<nRequests;i++)
    {
        vmReq = getVms(i);
        EV_INFO << "    +RQ[ Type:"  << vmReq.strVmType << " | MaxStart:  " << vmReq.maxStartTime_t1 << " | RentTime: " << vmReq.nRentTime_t2 << " | Id: " << vmReq.strVmId << "]" <<endl;

        nResponses = vmReq.responseList.size();
        //Print the responses
        for(int j=0;j<nResponses;j++)
        {
            vmRes = vmReq.responseList.at(j);
            EV_INFO << "    +RSP(" << j << ")[ Ip:  " << vmRes.strIp << " | RentStart: " << vmRes.startTime << " | Price:" << vmRes.nPrice << "]" <<endl;
        }
    }
    EV_INFO << "SM_UserVM::printUserVM - End" << endl;
}
void SM_UserVM::createResponse(int nIndex, bool bResOk, int nTime, std::string strIp, int nPrice)
{
    VM_Response vmRes;

    if(nIndex < getVmsArraySize())
      {
        if(bResOk)
          {
            vmRes.nOperationResult = 1;
            vmRes.strIp = strIp;
            vmRes.startTime = nTime;
            vmRes.nPrice = nPrice;
            EV_INFO << "    ~+RSP(" << nIndex << ")[ Ip:  " << vmRes.strIp << " | RentStart: " << vmRes.startTime << " | Price:" << vmRes.nPrice << "]" <<endl;
          }
        else
          {
            vmRes.nOperationResult = 0;
          }

        //Introduce a new response
        getVms(nIndex).responseList.push_back(vmRes);

      }
    else
      {
        EV_FATAL << "    ~+RSP(" << nIndex << ") out of bounds! Max: " << getVmsArraySize() << endl;
      }
}
std::string SM_UserVM::getVmRequestType(int nIndex)
{
    std::string strRet;
    VM_Request vmReq;
    if(nIndex<getTotalVmsRequests())
    {
        vmReq=getVms(nIndex);

            strRet =  vmReq.strVmType;

    }else
    {
        EV_FATAL << "    ~+RSP(" << nIndex << ") out of bounds! Max: " << getTotalVmsRequests() << endl;
    }
    return strRet;
}
VM_Response* SM_UserVM::getResponse(int nIndexRq, int nIndexRes)
{
    VM_Response* pvmRes;

    pvmRes = nullptr;
    if(nIndexRq<getTotalVmsRequests())
    {
        VM_Request& vmReq=getVms(nIndexRq);
        if(nIndexRes<vmReq.responseList.size())
        {
            //vmRes= vmReq.responseList.at(nIndexRes);
            pvmRes = &(vmReq.responseList.at(nIndexRes));
        }
    }

    return pvmRes;
}
bool SM_UserVM::hasResponse(int nIndexRq, int nIndexRes)
{
    bool bFound;
    int nRentTime;
    VM_Request vmReq;
    VM_Response vmRes;

    nRentTime=0;
    bFound = false;

    if(nIndexRq<getTotalVmsRequests())
    {
        vmReq=getVms(nIndexRq);
        if(nIndexRes<vmReq.responseList.size())
        {
            //vmRes= vmReq.responseList.at(nIndexRes);
            bFound=true;
        }
    }

    return bFound;
}

int SM_UserVM::getRentTime(int nIndex)
{
    int nRentTime;

    nRentTime=0;

    if(nIndex<getTotalVmsRequests())
    {
        nRentTime=getVms(nIndex).nRentTime_t2;
    }

    return nRentTime;
}
int SM_UserVM::getMaxStartTime(int nIndex)
{
    int nMaxStartTime;

    nMaxStartTime=0;

    if(nIndex<getTotalVmsRequests())
    {
        nMaxStartTime=getVms(nIndex).maxStartTime_t1;
    }

    return nMaxStartTime;
}
int SM_UserVM::getMaxSubscribetime(int nIndex)
{
    int nMaxSubTime;

    nMaxSubTime=0;

    if(nIndex<getTotalVmsRequests())
    {
        nMaxSubTime=getVms(nIndex).maxSubscriptionTime_t4;
    }

    return nMaxSubTime;
}
void SM_UserVM::clearResponses()
{
    //TODO: completar
}
void SM_UserVM::setTimeoutSubscribeMsg(SM_UserVM_Finish* pMsg)
{
    this->pMsgTimeoutSub = pMsg;
}
SM_UserVM_Finish* SM_UserVM::getTimeoutSubscribeMsg()
{
    return this->pMsgTimeoutSub;
}
/*
 //std::string getVmIp(int nIndex);
 * std::string SM_UserVM::getVmIp(int nIndex)
{
    VM_Request vmReq;
    std::string strIp;

    strIp="";
    if(nIndex<getTotalVmsRequests())
    {
        //vmReq = getVms(nIndex);
        //vmReq.responseList.
       // strIp=getVms(nIndex).strIp;
    }

    return strIp;
}
 */
