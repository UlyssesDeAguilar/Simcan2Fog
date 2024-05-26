#include "CloudUserInstance.h"

CloudUserInstance::CloudUserInstance(const CloudUser *ptrUser,
                                     unsigned int totalUserInstance,
                                     unsigned int userNumber,
                                     int currentInstanceIndex,
                                     int totalUserInstances)
    : UserInstance(ptrUser, userNumber, currentInstanceIndex, totalUserInstances)
{
    numFinishedVMs = 0;
    numActiveSubscriptions = 0;
    nId = totalUserInstance;
    requestVmMsg = nullptr;
    bTimeout_t2 = bTimeout_t4 = bFinished = false;

    // Init all instance times to 0
    times = {0};

    if (ptrUser != nullptr)
    {
        // Include VM instances
        int instanceIndex = 0;
        int totalVm = getNumVms(ptrUser);
        vmGroupedInstances.reserveCollections(totalVm);

        for (auto const &vm : ptrUser->allVMs())
        {
            VmInstanceType extrinsicState;
            extrinsicState.base = vm->getVmBase();
            extrinsicState.rentingTime = vm->getRentTime();
            extrinsicState.userId = this->id;
            extrinsicState.numInstances = vm->getNumInstances();

            const VmInstanceType *refType = &vmGroupedInstances.new_collection(extrinsicState);

            for (int i = 0; i < vm->getNumInstances(); i++)
            {
                VmInstance &lastElement = vmGroupedInstances.emplace_back(refType, instanceIndex++, totalVm);
                vmIdMap[lastElement.getId()] = vmGroupedInstances.flattened().size() - 1;
            }
        }

        //processApplicationCollection();
        if (getTotalVMs() < getNumberAppCollections())
            throw std::logic_error("There must be at least a VmInstance for each AppCollection");
    }
}

int CloudUserInstance::getNumVms(const CloudUser *user)
{
    int numVms = 0;

    for (const auto &vm : user->allVMs())
        numVms += vm->getNumInstances();

    return numVms;
}

std::string CloudUserInstance::toString(bool includeAppsParameters, bool includeVmFeatures)
{
    std::ostringstream info;
    int i;

    info << id << "\n";

    // Prints applications
    i = 0;
    for (const auto &appCollection : applications)
        info << "\t\tAppCollection[" << i++ << "] -> " << appCollection->toString(includeAppsParameters);

    /*info << "# Vm Instances:" << vmInstances.size() << "\n";
    i = 0;
    for (auto const &instance : )
        info << "\t\t  - VMs[" << i++ << "]: " << instance << " of type " << instance.getVmType() << "\n";

    if (includeVmFeatures)
        info << "\t\t\t Features:" << *vmInstances[0].getVmBase() << "\n";*/

    info << "\n";

    return info.str();
}

bool CloudUserInstance::operator<(const CloudUserInstance &other) const
{
    return this->times.arrival2Cloud < other.times.arrival2Cloud;
}

void CloudUserInstance::startExecution()
{
    times.initExec = simTime().inUnit(SIMTIME_S);
}

void CloudUserInstance::startSubscription()
{
    // If first then start recording the time
    if (numActiveSubscriptions < 1)
    {
        bSubscribe = true;
        times.initWaitTime = simTime().inUnit(SIMTIME_S);
    }

    // Count the subscription
    numActiveSubscriptions++;
}

void CloudUserInstance::endSubscription()
{
    numActiveSubscriptions--;

    // If it's the last subscription
    if (numActiveSubscriptions < 1)
        times.waitTime = (times.waitTime + simTime().inUnit(SIMTIME_S)) - times.initWaitTime;
}

void CloudUserInstance::updateVmInstanceStatus(const char *vmId, tVmState state)
{
    VmInstance &instance = vmGroupedInstances.flattenedForUpdate().at(vmIdMap.at(vmId));
    tVmState stateOld = instance.getState();

    if (stateOld != vmFinished && state == vmFinished)
        numFinishedVMs++;
    else if (stateOld == vmFinished && state != vmFinished)
        numFinishedVMs--;

    instance.setState(state);

    if (allVmsFinished())
        finish();
}

void CloudUserInstance::updateVmInstanceStatus(const SM_UserVM *request, tVmState state)
{
    for (auto &instance : vmGroupedInstances.flattenedForUpdate())
    {
        tVmState stateOld = instance.getState();

        if (stateOld != vmFinished && state == vmFinished)
            numFinishedVMs++;
        else if (stateOld == vmFinished && state != vmFinished)
            numFinishedVMs--;

        instance.setState(state);

        if (allVmsFinished())
            finish();
    }
}

void CloudUserInstance::finish()
{
    times.endTime = simTime().inUnit(SIMTIME_S);
    bFinished = true;
}