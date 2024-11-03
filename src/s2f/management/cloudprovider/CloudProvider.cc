#include "CloudProvider.h"
using namespace inet;

Define_Module(CloudProvider);

void CloudProvider::initialize()
{
  cModule *module = getModuleByPath("simData.manager");
  dataManager = check_and_cast<DataManager *>(module);
  dispatchPriority = par("dispatchPriority");
  listenerGate = gate("listener")->getId();
}

void CloudProvider::handleMessage(cMessage *msg)
{
  // Check if it's a queue time out
  if (msg->isSelfMessage())
  {
    handleSubscriptionTimeout(msg);
    return;
  }
  else if (msg->getArrivalGate()->getId() == listenerGate)
  {
    handleNodeUpdate(check_and_cast<Packet *>(msg));
    return;
  }

  // Then it should be a request
  auto sm = check_and_cast<SM_UserVM *>(msg);

  switch (sm->getOperation())
  {
  case SM_VM_Req:
    handleVmRequest(sm);
    break;
  case SM_VM_Sub:
    handleVmSubscription(sm);
    break;
  default:
    error("Unexpected message kind");
    break;
  }
}

void CloudProvider::handleNodeUpdate(Packet *update)
{
  auto nodeEvent = check_and_cast<const NodeEvent *>(update->peekData().get());
  auto iter = nodeMap.find(nodeEvent->getNodeTopic());

  if (iter == nodeMap.end())
  {
    // If the node wasn't registered before
    Node newNode;
    newNode.topic = nodeEvent->getNodeTopic();
    newNode.availableCores = nodeEvent->getAvailableCores();

    nodePool.push_back(newNode);
    coresMap[newNode.availableCores].insert(nodePool.size()-1);
    nodeMap[newNode.topic] = nodePool.size()-1;

    EV << "(CP) Added node to resource pool with topic: " << newNode.topic
       << " with: " << newNode.availableCores << " available cores";
  }
  else
  {
    // Update the node
    Node &nodeInfo = nodePool[iter->second];
    nodeInfo.availableCores = nodeEvent->getAvailableCores();

    EV << "(CP) Updated: " << nodeInfo.topic
       << " with: " << nodeInfo.availableCores << " available cores";
  }

  // As the invariants have changed, check the queue to see if a deployment is now possible
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

  const opp_string *destinationTopic = selectNode(request);
  // No suitable node found
  if (destinationTopic == nullptr)
  {
    rejectVmRequest(request);
    return;
  }

  // Forward to corresponding topic
  EV << "Request by user: " << request->getUserId() << " dispatched to node topic: " << *destinationTopic << "\n";
  request->setAutoSourceTopic(false);
  request->setDestinationTopic(destinationTopic->c_str());
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

const opp_string *CloudProvider::selectNode(const SM_UserVM *request)
{
  uint64_t totalCores = calculateRequestedCores(request);
  auto iter = coresMap.lower_bound(totalCores);

  // Couldn't find a node with greater or equal core count available
  if (iter == coresMap.end())
    return nullptr;

  // Retrieve the first node in the set
  auto first = iter->second.begin();
  if (first == iter->second.end())
    error("Empty pool!");

  //Node *node = const_cast<Node *>(*first);
  Node &node = nodePool[*first];

  // Update
  node.availableCores -= totalCores;

  // Insert into map (if no pool existing then create one)
  coresMap[node.availableCores].insert(*first);
  iter->second.erase(first);

  // If the pool got to zero then erase
  if (iter->second.size() == 0)
    coresMap.erase(iter);
  
  EV << "Estimating " << totalCores << " allocated for " << node.topic << "\n";
  EV << "New available core count: " << node.availableCores << "\n";

  return &node.topic;
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
  const opp_string *topic = nullptr;

  while (queue.size() > 0)
  {
    QueueElement &element = queue.front();
    topic = selectNode(element.request);

    if (topic)
    {
      cancelAndDelete(element.timeOut);
      element.request->setAutoSourceTopic(false);
      element.request->setDestinationTopic(topic->c_str());
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
  if (fromPos != string::npos && toPos != string::npos && fromPos < toPos)
  {
    std::string userType = userId.substr(fromPos + 1, toPos - fromPos - 1);
    return dataManager->searchUser(userType);
  }

  return nullptr;
}
