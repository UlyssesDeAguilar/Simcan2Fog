/*
 * SM_UserVM.cc
 *
 *  Created on: Apr 6, 2017
 *      Author: pablo
 */

#include "../messages/SM_UserVM.h"

#include <algorithm>
#include "../management/utils/LogUtils.h"

void SM_UserVM::copy(const SM_UserVM &other)
{
  // Base info already copied from superclass!
  // Copy the requests vector
  this->vmRequests = other.vmRequests;
}

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
  // Find vm by id
  auto selector = [vmId](const VM_Request &vm) -> bool
  { return vmId == vm.vmId; };

  auto iter = std::find_if(vmRequests.begin(), vmRequests.end(), selector);

  // If found then construct
  if (iter != vmRequests.end())
  {
    auto duplicate = new SM_UserVM();
    duplicate->SM_UserVM_Base::operator=(*this);
    duplicate->appendVm(*iter);
  }

  return nullptr;
}

void SM_UserVM::createNewVmRequest(const std::string &type, const std::string &instanceId, const VM_Request::InstanceRequestTimes &times)
{
  // Fill the request
  vmRequests.emplace_back();
  VM_Request &vmReq = vmRequests.back();
  vmReq.vmId = instanceId;
  vmReq.vmType = type;
  vmReq.times = times;
}

std::ostream &operator<<(std::ostream &os, const SM_UserVM &obj)
{
  os << "User id:        " << obj.userId << '\n';
  os << "Total requests: " << obj.vmRequests.size() << '\n';

  for (const auto &request : obj.vmRequests)
    os << request << '\n';

  return os;
}

void SM_UserVM::createResponse(int index, bool accepted, double time, const std::string &ip, int price)
{
  auto &response = vmRequests.at(index).response;

  if (accepted)
    response.state = VM_Response::ACCEPTED;
  else
    response.state = VM_Response::REJECTED;

  response.startTime = time;
  response.ip = ip;
  response.price = price;
}

bool SM_UserVM::getResponse(int index, VM_Response **const response)
{
  auto &r = vmRequests.at(index).response;
  if (r.state == VM_Response::ACCEPTED)
  {
    *response = &r;
    return true;
  }
  else
  {
    *response = nullptr;
    return false;
  }
}
