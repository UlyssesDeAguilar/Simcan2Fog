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
private:
     std::vector<VM_Request> vmRequests;
     void copy(const SM_UserVM &other);

public:
     SM_UserVM(const char *name = "SM_UserVM", short kind = 0);
     SM_UserVM(const SM_UserVM &other) : SM_UserVM_Base(other) { copy(other); }

     SM_UserVM &operator=(const SM_UserVM &other);

     virtual SM_UserVM *dup() const override { return new SM_UserVM(*this); }

     /**
      * @brief The duplicate only contains the requested vm
      * @param vmId The virtual machine id
      * @return SM_UserVM* The duplicated message or nullptr if not found
      */
     SM_UserVM *dup(const std::string &vmId) const;

     virtual ~SM_UserVM() = default;

     /**
      * @brief Create a New Vm Request object
      *
      * @param type The virtual machine type
      * @param instanceId The virtual machine (instance) id
      * @param times The request times for the instance
      */
     void createNewVmRequest(const std::string &type, const std::string &instanceId, const VM_Request::InstanceRequestTimes &times);

     void createResponse(int nIndex, bool accepted, double time, const std::string &ip, int price);

     /**
      * @brief Get the response for a given VM request
      *
      * @param index The position of the VM request
      * @param response Pointer to a pointer that will hold the reference to the response
      * @return true If there is a response
      * @return false If there isn't a response (*response is nullptr also)
      */
     bool getResponse(int index, VM_Response **const response);

     VM_Request::InstanceRequestTimes &getInstanceRequestTimes(int index) { return vmRequests.at(index).times; }

     SM_UserVM_Finish *getTimeoutSubscribeMsg() { return pMsgTimeoutSub; }
     void setTimeoutSubscribeMsg(SM_UserVM_Finish *pMsg) { this->pMsgTimeoutSub = pMsg; }

     virtual void insertVm(const VM_Request &vm) override { vmRequests.push_back(vm); }
     virtual void setVmArraySize(size_t size) override { vmRequests.resize(size); }
     virtual size_t getVmArraySize() const override { return vmRequests.size(); }
     virtual const VM_Request &getVm(size_t k) const override { return vmRequests.at(k); }

     friend std::ostream &operator<<(std::ostream &os, const SM_UserVM &obj);

private:
     virtual void setVm(size_t k, const VM_Request &vm) override {}
     virtual void insertVm(size_t k, const VM_Request &vm) override{};
     virtual void eraseVm(size_t k) override{};
     SM_UserVM_Finish *pMsgTimeoutSub;
};

#endif /* SM_USERVM_H_ */
