#include "UserGeneratorBase.h"

static void parse_swf(std::vector<Job_t> *jobs, const char *swf_file);

UserGeneratorBase::~UserGeneratorBase()
{
}

void UserGeneratorBase::initialize()
{
    // Init super-class
    ManagerBase::initialize();

    EV_INFO << "UserGeneratorBase::initialize - Init\n";

    // Init module parameters
    startDelay = par("startDelay");
    distribution = &par("distribution");
    intervalBetweenUsers = par("intervalBetweenUsers");
    shuffleUsers = par("shuffleUsers");
    showUserInstances = par("showUserInstances");
    strUserTraceFilePath = par("userTraceFilePath").stringValue();
    traceStartTime = par("traceStartTime");
    traceEndTime = par("traceEndTime");
    userTraceMaxVms = par("userTraceMaxVms");

    timeoutsTemplate =
        {
            .maxStartTime = par("maxStartTime_t1"),
            .rentTime = par("nRentTime_t2"),
            .maxSubTime = par("maxSubTime_t3"),
            .maxSubscriptionTime = par("maxSubscriptionTime")};

    activeCycles = par("activeCycles");
    numberOfCycles = par("numberOfCycles");
    durationOfCycle = par("durationOfCycle");
    isolateCycle = par("isolateCycle");
    cycleDistribution = &par("cycleDistribution");

    // Gates
    fromCloudProviderGate = gate("fromCloudProvider");
    toCloudProviderGate = gate("toCloudProvider");

    EV_INFO << "UserGeneratorBase::initialize - Generating users before simulation\n";

    groupOfUsers.clear();
    userInstances.clear();

    nUserInstancesFinished = 0;
    nUserIndex = 0;

    // Create user instances
    generateUsersBeforeSimulationStarts();
    parseTraceFile();

    // Show generated users instances
    if (showUserInstances)
    {
        EV_DEBUG << usersIstancesToString();
    }

    EV_INFO << "UserGeneratorBase::initialize - Scheduling a message"
            << "\n";

    // Start execution!
    cMessage *waitToExecuteMsg = new cMessage(Timer_WaitToExecute.c_str());
    scheduleAt(simTime() + SimTime(startDelay), waitToExecuteMsg);

    EV_INFO << "UserGeneratorBase::initialize - End"
            << "\n";
}

cGate *UserGeneratorBase::getOutGate(cMessage *msg)
{
    // If msg arrives from cloud provider
    if (msg->getArrivalGate() == fromCloudProviderGate)
        return toCloudProviderGate;

    return nullptr;
}

void UserGeneratorBase::generateUsersBeforeSimulationStarts()
{
    std::vector<CloudUserInstance *> userInstancesLocal;
    unsigned int currentUserNumber, totalUserInstance;
    CloudUserInstance *newUser, *pUser;
    int nSize;

    // Init...
    currentUserNumber = totalUserInstance = 0;

    EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - Init\n";

    // Process each user type to generate the user instances
    auto vec = dataManager->getSimUsers();
    for (auto &user : *vec)
    {
        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 0\n";

        // New vector of user instances
        userInstancesLocal.clear();

        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 1\n";

        // Generate the corresponding instances of the current user type
        for (int currentUserInstance = 0; currentUserInstance < user->getNumInstances(); currentUserInstance++)
        {
            EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 2\n";

            // Create a new user instance
            newUser = createCloudUserInstance(user, totalUserInstance, currentUserNumber, currentUserInstance, user->getNumInstances());

            EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 3\n";

            // Insert current user instance into the corresponding vector
            userInstancesLocal.push_back(newUser);
            userHashMap[newUser->getId()] = newUser;

            EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 4\n";
            // update user instance
            totalUserInstance++;

            userInstances.push_back(newUser);
        }

        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 5\n";
        // Insert current user instances in the global users vector
        groupOfUsers.push_back(userInstancesLocal);

        // Process next user type
        currentUserNumber++;

        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 6\n";
    }

    generateUserArrivalTimes();

    EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - End\n";
}

void UserGeneratorBase::generateUserArrivalTimes()
{
    SimTime lastTime = 0;
    m_dInitSim = simTime() + SimTime(startDelay);

    if (shuffleUsers)
        generateShuffledUsers();

    for (auto &userInstance : userInstances)
    {
        auto userVm = createVmRequest(userInstance);

        if (userVm != nullptr)
            userInstance->setRequestVmMsg(userVm);

        // FIXME: Maybe this could be done better
        auto pUserInstanceTrace = dynamic_cast<CloudUserInstanceTrace *>(userInstance);
        if (pUserInstanceTrace == nullptr)
        {
            // Set init and arrival time!
            lastTime = getNextTime(userInstance, lastTime);
            InstanceTimes &times = userInstance->getInstanceTimesForUpdate();
            times.initTime = lastTime.inUnit(SIMTIME_S);
            times.arrival2Cloud = lastTime.inUnit(SIMTIME_S);
        }
    }
}

SimTime UserGeneratorBase::getNextTime(CloudUserInstance *pUserInstance, SimTime last)
{
    SimTime next;

    if (intervalBetweenUsers)
    {
        if (last > 0)
            next = SimTime(distribution->doubleValue()) + last;
        else
            next = m_dInitSim;
    }
    else if (activeCycles)
    {
        next = SimTime(distribution->doubleValue()) + m_dInitSim;
    }
    else
    {
        int cycle = 0;
        next = SimTime(distribution->doubleValue());

        if (numberOfCycles > 1)
        {
            do
            {
                cycle = cycleDistribution->intValue();
            } while (cycle < 1 || cycle > numberOfCycles);
            cycle--;
        }

        SimTime offset = cycle * durationOfCycle;

        while (((isolateCycle || cycle == 0) && next < 0) ||
               ((isolateCycle || cycle == numberOfCycles - 1) && next > durationOfCycle) ||
               next + offset < 0 || next + offset > durationOfCycle * numberOfCycles)
        {
            next = SimTime(distribution->doubleValue());
        }

        next = m_dInitSim + next + offset;
    }

    return next;
}

SM_UserVM *UserGeneratorBase::createVmRequest(CloudUserInstance *pUserInstance)
{
    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init\n";

    if (pUserInstance == nullptr)
    {
        EV_INFO << "UserGenerator::createNextVmRequest - The user instance is null\n";
        return nullptr;
    }

    const std::string &userId = pUserInstance->getId();
    pUserInstance->setRentTimes(timeoutsTemplate);

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - UserId: " << userId
             << " | maxStartTime_t1: " << timeoutsTemplate.maxStartTime
             << " | rentTime_t2: " << timeoutsTemplate.rentTime
             << " | maxSubTime: " << timeoutsTemplate.maxSubTime
             << " | MaxSubscriptionTime:" << timeoutsTemplate.maxSubscriptionTime << "\n";

    if (pUserInstance->getTotalVMs() <= 0 || userId.size() <= 0)
    {
        EV_TRACE << "WARNING! [UserGenerator] Collection or User-ID malformed\n";
        throw cRuntimeError("[UserGenerator] Collection or User-ID malformed!");
    }

    // Creation of the message
    auto pUserRet = createVmMessage();
    pUserRet->setUserId(userId.c_str());
    pUserRet->setIsResponse(false);
    pUserRet->setOperation(SM_VM_Req);

    for (const auto &vmInstance : pUserInstance->getAllVmInstances())
    {
        std::string vmType = vmInstance.getVmType();
        std::string instanceId = vmInstance.getId();
        // double dRentTime = vmCollection->getRentTime() * 3600; // From hours to seconds FIXME: Originally ignored by the timeoutsTemplate
        pUserRet->createNewVmRequest(vmType, instanceId, timeoutsTemplate);
    }

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End\n";
    return pUserRet;
}

void UserGeneratorBase::generateShuffledUsers()
{
    int nSize = userInstances.size();

    srand((int)33); // TODO: semilla. Comprobar si con las semillas del .ini se puede omitir esta. Así se controla mejor la aletoriedad solo desde el fichero .ini.
    EV_INFO << "UserGenerator::generateShuffledUsers - Init"
            << "\n";
    EV_INFO << "UserGenerator::generateShuffledUsers - instances size: " << nSize << "\n";

    for (int i = 0; i < nSize; i++)
    {
        int nRandom = rand() % nSize;
        std::iter_swap(userInstances.begin() + i,
                       userInstances.begin() + nRandom);
    }

    EV_INFO << "UserGenerator::generateShuffledUsers - End"
            << "\n";
}

std::string UserGeneratorBase::usersIstancesToString()
{
    // Main text
    std::ostringstream info;
    info << "\n"
         << groupOfUsers.size() << " types of user instances in " << getFullPath() << "\n\n";

    // For each entry in the users vector...
    for (const auto &users : groupOfUsers)
    {
        // Print user type only once
        if (users.size() > 0)
            info << "\tUser type:" << users[0]->getType() << "  -  #instances: " << users.size() << "\n\n";

        for (const auto &userInstance : users)
            info << "\t  + UserID: " << userInstance->toString(false, false) << "\n";
    }

    return info.str();
}

CloudUserInstance *UserGeneratorBase::createCloudUserInstance(const CloudUser *ptrUser, unsigned int totalUserInstance, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances)
{
    return new CloudUserInstance(ptrUser, totalUserInstance, userNumber, currentInstanceIndex, totalUserInstances);
}

SM_UserVM *UserGeneratorBase::createVmMessage()
{
    return new SM_UserVM();
}

CloudUserInstance *UserGeneratorBase::createNewUserFromTrace(Job_t jobIn, int totalUserInstance, int nCurrentNumber, int nUserInstance, int nTotalInstances)
{
    auto pApp = dataManager->searchApp("AppCPUIntensive");
    return new CloudUserInstanceTrace(jobIn, totalUserInstance, nCurrentNumber, nUserInstance, nTotalInstances, pApp);
}

CloudUser *UserGeneratorBase::createUserTraceType()
{
    return new CloudUser("UserTrace", 0);
}

// FIXME: This actually doesn't work
// Esto va directamente a otra clase
void UserGeneratorBase::parseTraceFile()
{
    SM_UserVM *userVm;
    std::string strPath;
    std::vector<Job_t> jobs;
    Job_t singleJob;
    CloudUserInstance *newUser;
    int nJobs, nTotalUserInstance;
    double dInitTime;
    CloudUser *currentUserObject;

    EV_DEBUG << "UserGeneratorBase::parseTraceFile - Init"
             << "\n";

    // DONE: Esto se debe sacar de un parametro omnet, con = par.
    // strPath = "/home/pablo/applics/omnetpp-5.0/projects/SIMCAN-2.0/src/Management/traces/LPC.swf";
    parse_swf(&jobs, strUserTraceFilePath);
    nJobs = jobs.size();
    nTotalUserInstance = 0;

    // Show the jobs info
    if (nJobs > 0)
    {
        currentUserObject = createUserTraceType();
        // userTypes.push_back(currentUserObject);

        EV_INFO << "UserGeneratorBase::parseTraceFile - Jobs successfully loaded: " << nJobs << "\n";

        // TODO: Ojo con poner esto a fuego. Debug
        // for(int i=0;i<nJobs;i++)
        for (int i = 0; i < nJobs; i++)
        {
            singleJob = jobs.at(i);
            EV_DEBUG << "UserGeneratorBase::parseTraceFile - Job#" << i << " [" << singleJob.size << " CPU]"
                     << "[" << singleJob.runtime / 3600 << "H]"
                     << "\n";

            if (singleJob.runtime > 0 && singleJob.size != -1 && singleJob.submit >= traceStartTime && singleJob.submit <= traceEndTime)
            {
                // Limit number of vms per user
                if (singleJob.size > userTraceMaxVms)
                    singleJob.size = userTraceMaxVms;

                // TODO: Aqui tendr�a que ser necesario hacer un matching entre el tipo de maquina
                // y los cpus requeridos por el usuario

                // Para el paper de SUPE, tomamos la referencia de 1 cpu= 1 vmsmall
                // New user type with the job, and ids.
                dInitTime = singleJob.submit + startDelay - traceStartTime;
                newUser = createNewUserFromTrace(singleJob, nTotalUserInstance, nTotalUserInstance, nTotalUserInstance, nJobs);

                // Init the times
                auto times = newUser->getInstanceTimesForUpdate();
                times.initTime = dInitTime;
                times.arrival2Cloud = dInitTime;

                userVm = createVmRequest(newUser);
                // Create VM request
                if (userVm != nullptr)
                    newUser->setRequestVmMsg(userVm);
                userHashMap[newUser->getId()] = newUser;
                EV_DEBUG << "UserGeneratorBase::parseTraceFile - User generated: " << newUser->getId() << "\n";
                // Aqui creo que falta algo, con respecto al otro generador
                userInstances.push_back(newUser);

                nTotalUserInstance++;

                if (EV_TRACE)
                    singleJob.write(std::cout, 1);
            }
        }
    }
    EV_DEBUG << "UserGeneratorBase::parseTraceFile - End"
             << "\n";
}

//------------------------------------------------------------------------------
// read jobs from the given SWFfile into the jobs vector, while copying all
// non-job lines to the standard output (this is the header).
//
// only "sane" jobs are read, the rest are filtered out. a sane job is a jon
// with a positive and and a nonnegative runtime.
//------------------------------------------------------------------------------
static void parse_swf(std::vector<Job_t> *jobs, const char *swf_file)
{
    // 1- open the SWFfile
    std::ifstream swf(swf_file);
    if (swf)
    {
        // 2- ready...
        jobs->clear();
        jobs->reserve(100000);

        // 3- go!
        int insane = 0;
        for (std::string line; !getline(swf, line).eof();)
        {
            Job_t job;
            if (job.read(line.c_str()))
                if (job.runtime >= 0 && job.size > 0) // sane job
                    jobs->push_back(job);
                else
                    insane++;
            else
                std::cout << line << "\n";
        }

        // 4- warn if filtered jobs
        if (insane > 0)
            fprintf(stderr,
                    "#\n"
                    "# WARNING: %d jobs were filtered out (size<1 or runtime<0)\n",
                    insane);
    }
}
