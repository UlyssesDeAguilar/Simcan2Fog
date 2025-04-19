/*
 * SM_UserVM.cc
 *
 *  Created on: Apr 6, 2017
 *      Author: pablo
 */

#include "s2f/messages/SM_UserVM.h"
#include "s2f/management/utils/LogUtils.h"

SM_UserVM &SM_UserVM::operator=(const SM_UserVM &other)
{
  if (this == &other)
    return *this;
  SM_UserVM_Base::operator=(other);
  copy(other);
  return *this;
}

SM_UserVM *SM_UserVM::dup(const std::string &vmId) const
{
  for (int i = 0, size = getVmArraySize(); i < size; i++)
  {
    if (getVm(i).vmId == vmId)
    {
      auto duplicate = new SM_UserVM();
      duplicate->SM_UserVM_Base::operator=(*this);
      duplicate->appendVm(getVm(i));
      return duplicate;
    }
  }
  return nullptr;
}

void SM_UserVM::createNewVmRequest(const std::string &type, const std::string &instanceId, const VM_Request::InstanceRequestTimes &times)
{
  // Fill the request
  VM_Request vmReq;
  vmReq.vmId = instanceId;
  vmReq.vmType = type;
  vmReq.times = times;
  appendVm(vmReq);
}

std::ostream &operator<<(std::ostream &os, const SM_UserVM &obj)
{
  os << "User id:        " << obj.userId << '\n';
  os << "Total requests: " << obj.getVmArraySize() << '\n';

  for (int i = 0, size = obj.getVmArraySize(); i < size; i++)
    os << obj.getVm(i) << '\n';

  return os;
}

void SM_UserVM::createResponse(int index, bool accepted, double time, const std::string &ip, int price)
{
  auto &response = getVmForUpdate(index).response;

  if (accepted)
    response.state = VM_Response::ACCEPTED;
  else
    response.state = VM_Response::REJECTED;

  response.startTime = time;
  response.ip = ip;
  response.price = price;
}

const VM_Response *SM_UserVM::getResponse(int index) const
{
  const VM_Request &request = getVm(index);
  if (request.response.state == VM_Response::ACCEPTED)
    return &request.response;
  else
    return nullptr;
}
