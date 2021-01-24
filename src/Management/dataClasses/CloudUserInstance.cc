#include "CloudUserInstance.h"


CloudUserInstance::CloudUserInstance (CloudUser    *ptrUser,
                                      unsigned int  totalUserInstance,
                                      unsigned int  userNumber,
                                      int           currentInstanceIndex,
                                      int           totalUserInstances)
                  : UserInstance(ptrUser, userNumber, currentInstanceIndex, totalUserInstances)
{

    UserVmReference* vmReference;
    std::map<std::string, int> offsetMap,
                               totalVmMap;
    int currentVm;
    numFinishedVMs = 0;
    numTotalVMs = 0;

    for (currentVm = 0; currentVm < ptrUser->getNumVirtualMachines(); currentVm++)
      {
        std::string strType = ptrUser->getVirtualMachine(currentVm)->getVmBase()->getType();
        if (offsetMap.find(strType) == offsetMap.end())
          {
            offsetMap[strType] = 0;
            totalVmMap[strType] = getNumVms(strType, ptrUser);
            numTotalVMs += totalVmMap[strType];
          }
      }

    // Include VM instances
    for (currentVm = 0; currentVm < ptrUser->getNumVirtualMachines(); currentVm++)
      {
        // Get current application
        vmReference = ptrUser->getVirtualMachine(currentVm);
        std::string strType = vmReference->getVmBase()->getType();
        int offset = offsetMap.at(strType),
            totalVm = totalVmMap.at(strType);

        // Insert a new collection of application instances
        insertNewVirtualMachineInstances (vmReference->getVmBase(), vmReference->getNumInstances(), vmReference->getRentTime(), totalVm, offset);

        offsetMap[strType] = offset + vmReference->getNumInstances();
      }

    processApplicationCollection();

    nId = totalUserInstance;

    requestVmMsg = nullptr;
    requestAppMsg = nullptr;
    subscribeVmMsg = nullptr;

    bTimeout_t2 = bTimeout_t4 = bFinished = false;
    dInitTime = dEndTime = dInitWaitTime = dWaitTime = 0.0;

}



CloudUserInstance::~CloudUserInstance() {
    virtualMachines.clear();
}

int CloudUserInstance::getNumVms(std::string strType, CloudUser *ptrUser)
{
    int numVms = 0;

    for (int currentVm = 0; currentVm < ptrUser->getNumVirtualMachines(); currentVm++)
      {
        UserVmReference* vmReference = ptrUser->getVirtualMachine(currentVm);

        if (vmReference->getVmBase()->getType().compare(strType) == 0)
            numVms += vmReference->getNumInstances();
      }

    return numVms;
}

void CloudUserInstance::insertNewVirtualMachineInstances (VirtualMachine* vmPtr, int numInstances, int nRentTime, int total, int offset){

    VmInstanceCollection* newVmCollection;

    newVmCollection = new VmInstanceCollection(vmPtr, this->userID, numInstances, nRentTime, total, offset);
    virtualMachines.push_back(newVmCollection);
}


std::string CloudUserInstance::toString (bool includeAppsParameters, bool includeVmFeatures){

    std::ostringstream info;
    int i;

        info << userID << std::endl;

            // Parses applications
            for (i=0; i<applications.size(); i++){
                info << "\t\tAppCollection[" << i << "] -> " << applications.at(i)->toString(includeAppsParameters);
            }

            // Parses VMs
            for (i=0; i<virtualMachines.size(); i++){
                info << "\t\tVM set[" << i << "] -> " << virtualMachines.at(i)->toString(includeVmFeatures);
            }

    return info.str();
}

int CloudUserInstance::getAppCollectionSize(int nIndex)
{
    int size = -1;

    if(nIndex<applications.size())
    {
        size = applications.at(nIndex)->getNumInstances();
    }

    return size;
}

AppInstance* CloudUserInstance::getAppInstance(int nIndex)
{
    AppInstance* pAppRet;

    pAppRet = nullptr;

    if(nIndex<appInstances.size())
    {
        pAppRet = appInstances.at(nIndex);
    }

    return pAppRet;
}

VmInstanceCollection* CloudUserInstance::getVmCollection(int nCollection)
{
    VmInstanceCollection *pVmCollection = nullptr;

    if(nCollection < virtualMachines.size())
        pVmCollection = virtualMachines.at(nCollection);

    return pVmCollection;
}

void CloudUserInstance::setRentTimes(int maxStartTime_t1, int nRentTime_t2, int maxSubTime_t3, int maxSubscriptionTime_t4)
{
    this->maxStartTime_t1 = maxStartTime_t1;
    this->nRentTime_t2 =  nRentTime_t2;
    this->maxSubTime_t3 = maxSubTime_t3;
    this->maxSubscriptionTime_t4 = maxSubscriptionTime_t4;
}

int CloudUserInstance::getId() const {
    return nId;
}

void CloudUserInstance::setId(int id) {
    nId = id;
}

VmInstance* CloudUserInstance::getNthVm(int index) {
    VmInstance *vm = nullptr;
    int offset = 0;

    for (std::vector<VmInstanceCollection*>::iterator colIt = virtualMachines.begin(); colIt != virtualMachines.end(); ++colIt)
      {
        VmInstanceCollection* pCol = *colIt;
        for (int i = 0; i < pCol->getNumInstances(); i++)
          {
            if (offset + i == index)
              {
                vm = pCol->getVmInstance(i);
                break;
              }
          }
        offset += pCol->getNumInstances();
      }

    return vm;
}

void CloudUserInstance::processApplicationCollection()
{
    int vm_size = virtualMachines.size();

    for(int i = 0; i < applications.size(); i++)
      {
        AppInstanceCollection *pCol = applications.at(i);

        for(int j = 0; j < pCol->getNumInstances(); j++)
          {
            AppInstance *pApp = pCol->getInstance(j);
            VmInstance *pVm = getNthVm(i);
            if (pVm != nullptr)
              {
                pApp->setVmInstanceId(pVm->getVmInstanceId());
                appInstances.push_back(pApp);
              }
            else
              {
                EV_FATAL << "Error while setting vmId to appInstance. The number of App collections must match the number of VMs";
              }
          }

      }
}

SimTime CloudUserInstance::getEndTime() const
{
   return dEndTime;
}

void CloudUserInstance::setEndTime(SimTime endTime)
{
   dEndTime = endTime;
}

SimTime CloudUserInstance::getInitTime() const
{
   return dInitTime;
}

void CloudUserInstance::setInitTime(SimTime initTime)
{
   dInitTime = initTime;
}

bool CloudUserInstance::isFinished() const
{
   return bFinished;
}

void CloudUserInstance::setFinished(bool finished)
{
   bFinished = finished;
}

SimTime CloudUserInstance::getArrival2Cloud() const
{
   return dArrival2Cloud;
}

void CloudUserInstance::setArrival2Cloud(SimTime arrival2Cloud)
{
   dArrival2Cloud = arrival2Cloud;
}

void CloudUserInstance::setTimeoutMaxStart()
{
   bTimeout_t2 = true;
}

void CloudUserInstance::setTimeoutMaxRentTime()
{
   bTimeout_t2 = true;
}
void CloudUserInstance::setTimeoutMaxSubscribed()
{
   bTimeout_t4 = true;
}
bool CloudUserInstance::isTimeout()
{
   return  (bTimeout_t2 || bTimeout_t4);
}
bool CloudUserInstance::isTimeoutMaxRent()
{
   return bTimeout_t2;
}

bool CloudUserInstance::isTimeoutSubscribed()
{
   return bTimeout_t4;
}
SimTime CloudUserInstance::getInitExecTime() const
{
    return this->dInitExec;
}
void CloudUserInstance::setInitExecTime(SimTime dExec)
{
    this->dInitExec = dExec;
}
void CloudUserInstance::setSubscribe(bool bSubscribe)
{
    this->bSubscribe = bSubscribe;
}
bool CloudUserInstance::hasSubscribed()
{
    return this->bSubscribe;
}

bool CloudUserInstance::operator <(const CloudUserInstance &other) const {
    return this->getArrival2Cloud() < other.getArrival2Cloud() ;
}

bool CloudUserInstance::allVmsFinished()
{
    return numTotalVMs <= numFinishedVMs;
}


void CloudUserInstance::addFinishedVMs(int newFinished)
{
    numFinishedVMs += newFinished;
}

int CloudUserInstance::getTotalVMs() const
{
    return numTotalVMs;
}

const SimTime& CloudUserInstance::getWaitTime() const {
    return dWaitTime;
}

void CloudUserInstance::setWaitTime(const SimTime &dWaitTime) {
    this->dWaitTime = dWaitTime;
}

const SimTime& CloudUserInstance::getInitWaitTime() const {
    return dInitWaitTime;
}

void CloudUserInstance::setInitWaitTime(const SimTime &dInitWaitTime) {
    this->dInitWaitTime = dInitWaitTime;
}
