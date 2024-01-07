#include "CloudUserInstance.h"

CloudUserInstance::CloudUserInstance(CloudUser *ptrUser,
                                     unsigned int totalUserInstance,
                                     unsigned int userNumber,
                                     int currentInstanceIndex,
                                     int totalUserInstances)
    : UserInstance(ptrUser, userNumber, currentInstanceIndex, totalUserInstances)
{
    std::map<std::string, int> offsetMap, totalVmMap;

    numFinishedVMs = 0;
    numTotalVMs = 0;
    numActiveSubscriptions = 0;
    nId = totalUserInstance;
    requestVmMsg = nullptr;
    requestAppMsg = nullptr;
    subscribeVmMsg = nullptr;

    bTimeout_t2 = bTimeout_t4 = bFinished = false;

    // Init all instance times to 0
    times = {0.0};

    if (ptrUser != nullptr)
    {

        for (auto const &vm : ptrUser->allVMs())
        {
            std::string type = vm->getVmBase()->getType();
            if (offsetMap.find(type) == offsetMap.end())
            {
                offsetMap[type] = 0;
                totalVmMap[type] = getNumVms(type, ptrUser);
                numTotalVMs += totalVmMap[type];
            }
        }

        // Include VM instances
        for (auto const &vm : ptrUser->allVMs())
        {
            std::string type = vm->getVmBase()->getType();

            int offset = offsetMap.at(type),
                totalVm = totalVmMap.at(type);

            // Insert a new collection of application instances
            insertNewVirtualMachineInstances(vm->getVmBase(), vm->getNumInstances(), vm->getRentTime(), totalVm, offset);

            offsetMap[type] = offset + vm->getNumInstances();
        }

        processApplicationCollection();
    }
}

CloudUserInstance::~CloudUserInstance()
{
    virtualMachines.clear();
}

int CloudUserInstance::getNumVms(std::string vmType, CloudUser *user)
{
    int numVms = 0;

    for (const auto &vm: user->allVMs())
        if (vm->getVmBase()->getType().compare(type) == 0)
            numVms += vm->getNumInstances();

    return numVms;
}

void CloudUserInstance::insertNewVirtualMachineInstances(VirtualMachine *vmPtr, int numInstances, int nRentTime, int total, int offset)
{
    auto newVmCollection = new VmInstanceCollection(vmPtr, this->id, numInstances, nRentTime, total, offset);
    virtualMachines.push_back(newVmCollection);
}

std::string CloudUserInstance::toString(bool includeAppsParameters, bool includeVmFeatures)
{
    std::ostringstream info;
    int i;

    info << id << "\n";

    // Prints applications
    i = 0;
    for (const auto &appCollection : applications)
        info << "\t\tAppCollection[" << i << "] -> " << appCollection->toString(includeAppsParameters);

    // Prints VMs
    i = 0;
    for (const auto &vmCollection : virtualMachines)
        info << "\t\tVM set[" << i << "] -> " << vmCollection->toString(includeVmFeatures);

    return info.str();
}

AppInstance *CloudUserInstance::getAppInstance(int nIndex)
{
    AppInstance *pAppRet;

    pAppRet = nullptr;

    if (nIndex < appInstances.size())
    {
        pAppRet = appInstances.at(nIndex);
    }

    return pAppRet;
}

VmInstance *CloudUserInstance::getNthVm(int index)
{
    int offset = 0;

    // Simple check, negative indexes don't represent a valid position
    if (index < 0)
        return nullptr;

    // We iterate through the collections
    for (const auto &vmCollection : virtualMachines)
    {
        int nInstances = vmCollection->getNumInstances();

        // If this happens, then the current collection contains the element at the requested index
        if (offset + nInstances > index)
        {
            return &vmCollection->getVmInstance(index - offset);
        }
        else
            offset += nInstances; // Otherwise keep advancing
    }

    // If after iterating throughout the collections we couldn't find the requested index
    return nullptr;
}

void CloudUserInstance::processApplicationCollection()
{
    int vm_size = virtualMachines.size();

    int i = 0;
    for (auto const &appCollection : applications)
    {
        for (int j = 0; j < appCollection->getNumInstances(); j++)
        {
            AppInstance *pApp = appCollection->getInstance(j);
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
        i++;
    }
}

bool CloudUserInstance::operator<(const CloudUserInstance &other) const
{
    return this->times.arrival2Cloud < other.times.arrival2Cloud;
}

void CloudUserInstance::startSubscription()
{
    if (numActiveSubscriptions < 1)
        times.initWaitTime = simTime();

    numActiveSubscriptions++;
}

void CloudUserInstance::endSubscription()
{
    // FIXME: It doesn't implode because waitTime it's initialized to 0, logically this is weird
    SimTime stWaitTime;
    numActiveSubscriptions--;
    if (numActiveSubscriptions < 1)
    {
        stWaitTime = times.waitTime;
        times.waitTime = stWaitTime + simTime() - times.initWaitTime;
    }
}
