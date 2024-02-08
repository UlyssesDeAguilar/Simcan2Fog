#include "CloudManagerBase.h"

CloudManagerBase::~CloudManagerBase()
{
    selfHandlers.clear();
    requestHandlers.clear();
    responseHandlers.clear();
}

void CloudManagerBase::initialize()
{
    // Init super-class
    cSIMCAN_Core::initialize();

    // Prepare the state
    bFinished = false;
    acceptResponses = true;

    // Locate and bind dataManager, will fail if not found or incorrectly placed
    cModule * module = getModuleByPath("simData.manager");
    dataManager = check_and_cast<DataManager *>(module);

    // Maybe this step should be moved onto the class constructor!
    initializeSelfHandlers();
    initializeRequestHandlers();
    initializeResponseHandlers();
}

void CloudManagerBase::processSelfMessage(cMessage *msg)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Auto Event\n";

    try
    {
        auto callback = selfHandlers.at(msg->getKind());
        callback(msg);
    }
    catch (std::out_of_range &e)
    {
        error("Unknown self message [%d]", msg->getKind());
        cancelAndDelete(msg);
    }
}

void CloudManagerBase::processRequestMessage(SIMCAN_Message *sm)
{
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Request Message\n";

    auto userControl = dynamic_cast<SM_CloudProvider_Control *>(sm);

    if (userControl != nullptr)
    {
        EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received end of party\n";
        // Stop the checking process.
        bFinished = true;
        cancelAndDelete(userControl);
    }
    else
    {
        try
        {
            auto callback = requestHandlers.at(sm->getOperation());
            callback(sm);
        }
        catch (std::out_of_range &e)
        {
            EV_WARN << "Recieved request with code: " << sm->getOperation() << " which has no handler\n";
        }
    }
}

void CloudManagerBase::processResponseMessage(SIMCAN_Message *sm)
{
    if (!acceptResponses)
        error("This module cannot process response messages:%s", sm->contentsToString(showMessageContents, showMessageTrace).c_str());
    
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Received Request Message" << '\n';

    try
    {
        auto callback = responseHandlers.at(sm->getResult());
        callback(sm);
    }
    catch (std::out_of_range &e)
    {
        EV_WARN << "Recieved response with code: " << sm->getResult() << " which has no handler\n";
    }
}

const CloudUser *CloudManagerBase::findUserTypeById(const std::string &userId)
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

int CloudManagerBase::getTotalCoresByVmType(const std::string &vmType)
{
    auto vm = dataManager->searchVirtualMachine(vmType);
    return vm != nullptr ? vm->getNumCores() : 0;
}

int CloudManagerBase::calculateTotalCoresRequested(const SM_UserVM *request)
{
    int nRet = 0;
    if (request != nullptr)
    {
        int nRequestedVms = request->getVmArraySize();

        for (int i = 0; i < nRequestedVms; i++)
        {
            const VM_Request &vmRequest = request->getVm(i);
            nRet += getTotalCoresByVmType(vmRequest.vmType);
        }
    }
    EV_DEBUG << "User:" << request->getUserId() << " has requested: " << nRet << " cores\n";

    return nRet;
}

SM_UserVM_Finish *CloudManagerBase::scheduleVmMsgTimeout(Event eventType, std::string &userId, VM_Request &request, double timeOut)
{
    double trueTimeOut;
    auto msg = new SM_UserVM_Finish();

    // FIXME: msg->setName(); <-- It's good for the GUI !!
    msg->setKind(eventType);
    msg->setUserID(userId.c_str());
    msg->setStrVmId(request.vmId.c_str());
    msg->setStrVmType(request.vmType.c_str());

    // Select the "true" time out (allows overriding)
    trueTimeOut = timeOut == 0 ? request.times.rentTime.dbl() : timeOut;

    // Schedule it
    auto eventTime = simTime() + timeOut;
    scheduleAt(eventTime, msg);

    // Log the scheduling
    EV_INFO << "Scheduling timeout message: " << msg << " at " << eventTime << '\n';

    return msg;
}

SM_UserVM_Finish *CloudManagerBase::scheduleVmMsgTimeout(Event eventType, const SM_UserVM *request, double timeOut)
{
    auto msg = new SM_UserVM_Finish();

    // FIXME: msg->setName(); <-- It's good for the GUI !!
    msg->setKind(eventType);
    msg->setUserID(request->getUserId());
    msg->setStrVmId(request->getVmId());

    // Schedule it
    auto eventTime = simTime() + timeOut;
    scheduleAt(eventTime, msg);

    // Log the scheduling
    EV_INFO << "Scheduling timeout message: " << msg << " at " << eventTime << '\n';

    return msg;
}
