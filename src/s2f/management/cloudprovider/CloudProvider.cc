#include "CloudProvider.h"
using namespace inet;

Define_Module(CloudProvider);

void CloudProvider::initialize()
{
  cModule *module = getModuleByPath("simData.manager");
  dataManager = check_and_cast<DataManager *>(module);
  nodeDb = check_and_cast<NodeDb *>(getModuleByPath("^.nodeDb"));
  dispatchPriority = par("dispatchPriority");
}

void CloudProvider::handleMessage(cMessage *msg)
{
  // Check if it's a queue time out
  if (msg->isSelfMessage())
  {
    handleSubscriptionTimeout(msg);
    return;
  }

  // Then it should be a request
  auto sm = check_and_cast<SIMCAN_Message *>(msg);

  if (sm->getOperation() == SM_VM_Req)
    handleVmRequest(check_and_cast<SM_UserVM *>(msg));
  else if (sm->getOperation() == SM_VM_Sub)
    handleVmSubscription(check_and_cast<SM_UserVM *>(msg));
  else if (sm->getOperation() == SM_Node_Update)
    handleNodeUpdate(check_and_cast<NodeUpdate *>(msg));
  else
    error("Unexpected message kind with code %d", sm->getOperation());
}

void CloudProvider::handleNodeUpdate(NodeUpdate *update)
{
  // As the invariants have changed, check the queue to see if a deployment is now possible
  nodeDb->addOrUpdateNode(update->getReturnTopic(),
                          update->getZone(),
                          update->getAvailableCores(),
                          update->getAvailableRam(),
                          update->getAvailableDisk());
  attemptDispatching();
  delete update;
}

void CloudProvider::handleVmRequest(SM_UserVM *request)
{
  bool skip = false;

  if (dispatchPriority)
  {
    auto userType = check_and_cast_nullable<const CloudUserPriority *>(findUserTypeById(request->getUserId()));
    skip = (userType != nullptr) && (userType->getPriorityType() == Priority);
  }

  // If there are people in the queue and there is no reason to skip it
  if (queue.size() > 0 && !skip)
  {
    rejectVmRequest(request);
    return;
  }

  const char *destinationTopic = selectNode(request);

  // No suitable node found
  if (destinationTopic == nullptr)
  {
    rejectVmRequest(request);
    return;
  }

  // Forward to corresponding topic
  EV << "Request by user: " << request->getUserId() << " dispatched to node topic: " << *destinationTopic << "\n";
  request->setAutoSourceTopic(false);
  request->setDestinationTopic(destinationTopic);
  send(request, "queueOut");
}

void CloudProvider::handleVmSubscription(SM_UserVM *sm)
{
  SimTime currentTime = simTime();

  // Schedule it
  double dMaxSubscribeTime = sm->getInstanceRequestTimes(0).maxSubTime;
  auto event = new cMessage("Subscribe queue timeout");
  event->setContextPointer(sm);
  scheduleAt(currentTime + dMaxSubscribeTime, event);

  // Start recording queue wait time
  sm->setDStartSubscriptionTime(currentTime.dbl());
  queue.emplace_back(sm, event);

  EV_INFO << "Subscribing the VM request from user:" << sm->getUserId() << " with max sub time: " << dMaxSubscribeTime << '\n';
}

void CloudProvider::handleSubscriptionTimeout(cMessage *msg)
{
  // Recover context pointer which points to the request who reached the time out
  auto request = reinterpret_cast<SM_UserVM *>(msg->getContextPointer());
  delete msg;

  // Remove the item from the queue
  auto finder = [request](const QueueElement &e)
  { return e.request == request; };

  auto iter = std::find_if(queue.begin(), queue.end(), finder);
  if (iter == queue.end())
    error("Recieved timeout for request that doesn't exist in the queue");

  queue.erase(iter);

  EV_INFO << "The subscription of the user:  " << request->getUserId() << " has expired\n";

  request->setDEndSubscriptionTime(simTime().dbl());

  // Fill the message and send
  request->setIsResponse(true);
  request->setOperation(SM_VM_Sub);
  request->setResult(SM_APP_Sub_Timeout);
  request->setDestinationTopic(request->getReturnTopic());
  send(request, "queueOut");
}

const char *CloudProvider::selectNode(const SM_UserVM *request)
{
  const char *zone = request->getZone();
  const std::set<int> *nodesInZone = nodeDb->findNodesByZone(zone);
  const char *topic{};

  if (nodesInZone && (topic = selectNodeInPool(request, *nodesInZone)) != nullptr)
    return topic;

  EV << "No suitable nodes found in zone: " << zone << " defaulting to zone: " << defaultZone << "\n";
  nodesInZone = nodeDb->findNodesByZone(defaultZone);

  if (!nodesInZone)
    error("No nodes found in default zone! %s", defaultZone);
    
  if ((topic = selectNodeInPool(request, *nodesInZone)) != nullptr)
    return topic;
  
  return nullptr;
}

const char *CloudProvider::selectNodeInPool(const SM_UserVM *request, const std::set<int> &nodesInZone)
{
  uint64_t totalCores = calculateRequestedCores(request);
  for (auto &nodeId : nodesInZone)
  {
    auto &node = nodeDb->getNode(nodeId);
    if (node.availableCores >= totalCores)
    {
      const char *topic = nodeDb->getTopicForNode(node);
      EV << "Estimating " << totalCores << " allocated for node in: " << topic << "\n";
      node.availableCores -= totalCores;
      EV << "New available core count: " << node.availableCores << "\n";
      return topic;
    }
  }
  return nullptr;
}

uint64_t CloudProvider::calculateRequestedCores(const SM_UserVM *request)
{
  uint64_t total = 0;
  for (int i = 0; i < request->getVmArraySize(); i++)
  {
    const VM_Request &vmRequest = request->getVm(i);
    total += dataManager->searchVm(vmRequest.vmType)->getNumCores();
  }
  return total;
}

void CloudProvider::rejectVmRequest(SM_UserVM *userVM_Rq)
{
  EV_INFO << "Rejecting VM request from user:" << userVM_Rq->getUserId() << '\n';
  userVM_Rq->setIsResponse(true);
  userVM_Rq->setOperation(SM_VM_Req);
  userVM_Rq->setResult(SM_VM_Res_Reject);
  userVM_Rq->setDestinationTopic(userVM_Rq->getReturnTopic());
  send(userVM_Rq, "queueOut");
}

void CloudProvider::attemptDispatching()
{
  const char *topic = nullptr;

  while (queue.size() > 0)
  {
    QueueElement &element = queue.front();
    topic = selectNode(element.request);

    if (topic)
    {
      cancelAndDelete(element.timeOut);
      element.request->setAutoSourceTopic(false);
      element.request->setDestinationTopic(topic);
      send(element.request, "queueOut");
      queue.pop_front();
    }
    else
    {
      return;
    }
  }
}

const CloudUser *CloudProvider::findUserTypeById(const std::string &userId)
{
  size_t fromPos = userId.find_first_of(')');
  size_t toPos = userId.find_first_of('[');

  // If we could parse the user type within the Id
  if (fromPos != std::string::npos && toPos != std::string::npos && fromPos < toPos)
  {
    std::string userType = userId.substr(fromPos + 1, toPos - fromPos - 1);
    return dataManager->searchUser(userType);
  }

  return nullptr;
}
