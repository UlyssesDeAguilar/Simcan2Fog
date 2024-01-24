#include "../CloudProviderCost/CloudProviderCost.h"

#include "Management/utils/LogUtils.h"

Define_Module(CloudProviderCost);

void CloudProviderCost::initialize()
{
    EV_INFO << "CloudProviderFirstFit::initialize - Init" << endl;

    // Init super-class
    CloudProviderFirstFit::initialize();
}

void CloudProviderCost::initializeDataCentreCollection()
{
    datacentreCollection = new DataCentreInfoCollectionReservation();
}

void CloudProviderCost::initializeRequestHandlers()
{
    CloudProviderFirstFit::initializeRequestHandlers();

    requestHandlers[SM_APP_Req_Resume] = [this](SIMCAN_Message *msg) -> void
    { return handleExtendVmAndResumeExecution(msg); };
    requestHandlers[SM_APP_Req_End] = [this](SIMCAN_Message *msg) -> void
    { return handleEndVmAndAbortExecution(msg); };
}

int CloudProviderCost::parseDataCentresList()
{
    int result;
    const char *dataCentresListChr;

    dataCentresListChr = par("dataCentresList");
    DataCentreReservationListParser dataCentreParser(dataCentresListChr);
    result = dataCentreParser.parse();
    if (result == SC_OK)
    {
        dataCentresBase = dataCentreParser.getResult();
    }
    return result;
}

void CloudProviderCost::loadNodes()
{
    int nIndex;
    std::vector<int> reservedNodes;
    std::vector<int>::iterator it;

    it = reservedNodes.begin();

    for (nIndex = 0; nIndex < dataCentresBase.size(); nIndex++)
    {
        DataCentreReservation *dataCentreReservation = dynamic_cast<DataCentreReservation *>(dataCentresBase.at(nIndex));
        if (dataCentreReservation != nullptr)
        {

            reservedNodes.insert(it + nIndex, dataCentreReservation->getReservedNodes());
        }
    }

    DataCentreInfoCollectionReservation *datacentreCollectionReservation = dynamic_cast<DataCentreInfoCollectionReservation *>(datacentreCollection);

    if (datacentreCollectionReservation != nullptr)
    {
        datacentreCollectionReservation->setReservedNodes(reservedNodes);
    }

    CloudProviderFirstFit::loadNodes();
}

void CloudProviderCost::handleVmRequestFits(SIMCAN_Message *sm)
{
    SM_UserVM *userVM_Rq;
    bool bPriorityUser = false;

    userVM_Rq = dynamic_cast<SM_UserVM *>(sm);
    EV_INFO << LogUtils::prettyFunc(__FILE__, __func__) << " - Handle VM_Request" << endl;

    if (userVM_Rq == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong userVM_Rq. Null pointer or bad operation code!").c_str());

    auto pCloudUser = dynamic_cast<const CloudUserPriority *>(findUserTypeById(userVM_Rq->getUserID()));

    if (pCloudUser == nullptr)
        throw omnetpp::cRuntimeError(("[" + LogUtils::prettyFunc(__FILE__, __func__) + "] Wrong pCloudUser. Null pointer or wrong cloud user class!").c_str());

    userVM_Rq->printUserVM();
    // Check if is a VmRequest or a subscribe
    if (subscribeQueue.size() > 0 && pCloudUser->getPriorityType() != Priority)
        rejectVmRequest(userVM_Rq);
    else if (checkVmUserFit(userVM_Rq))
    {
        // acceptVmRequest(userVM_Rq);
        EV_FATAL << "OK" << endl;
    }
    else
    {
        EV_FATAL << "Fail" << endl;
        rejectVmRequest(userVM_Rq);
    }
}

void CloudProviderCost::handleExtendVmAndResumeExecution(SIMCAN_Message *sm)
{
    SM_UserAPP *userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);

    if (userAPP_Rq != nullptr)
    {
        userAPP_Rq->setIsResponse(false);
        sendRequestMessage(sm, toDataCentreGates[0]);
    }
    else
    {
        error("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());
    }
}

void CloudProviderCost::handleEndVmAndAbortExecution(SIMCAN_Message *sm)
{
    SM_UserAPP *userAPP_Rq = dynamic_cast<SM_UserAPP *>(sm);

    if (userAPP_Rq != nullptr)
    {
        userAPP_Rq->setIsResponse(false);
        sendRequestMessage(sm, toDataCentreGates[0]);
    }
    else
    {
        error("%s - Unable to cast msg to SM_UserAPP*. Wrong msg name [%s]?", LogUtils::prettyFunc(__FILE__, __func__).c_str(), sm->getName());
    }
}