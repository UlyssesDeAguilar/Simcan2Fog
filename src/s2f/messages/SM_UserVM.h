/*
 * SM_UserVM.h
 *
 *  Created on: Apr 6, 2017
 *      Author: pablo
 */

#ifndef SM_USERVM_H_
#define SM_USERVM_H_

#include "../messages/SM_UserVM_m.h"

class SM_UserVM : public SM_UserVM_Base
{
private:
     void copy(const SM_UserVM &other) {};

public:
     SM_UserVM(const char *name = "SM_UserVM", short kind = 0) : SM_UserVM_Base(name, kind) {}
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
      * @brief Get the response for the given index
      * @param index Index of the request
      * @return The response or nullptr if not found
      */
     const VM_Response *getResponse(int index) const;

     VM_Request::InstanceRequestTimes &getInstanceRequestTimes(int index) { return getVmForUpdate(index).times; }
     friend std::ostream &operator<<(std::ostream &os, const SM_UserVM &obj);
};

#endif /* SM_USERVM_H_ */
