#include "UserGeneratorBase.h"

static void parse_swf(vector<Job_t> *jobs, const char* swf_file);

UserGeneratorBase::~UserGeneratorBase(){
}


void UserGeneratorBase::initialize(){

    // Init super-class
    CloudManagerBase::initialize();

    EV_INFO << "UserGeneratorBase::initialize - Init" << endl;

    // Init module parameters
    startDelay = par ("startDelay");
    distribution = &par ("distribution");
    intervalBetweenUsers = par ("intervalBetweenUsers");
    shuffleUsers = par ("shuffleUsers");
    showUserInstances = par ("showUserInstances");
    strUserTraceFilePath = par ("userTraceFilePath").stringValue();
    traceStartTime = par ("traceStartTime");
    traceEndTime = par ("traceEndTime");
    userTraceMaxVms = par ("userTraceMaxVms");

    maxStartTime_t1 = par("maxStartTime_t1");
    nRentTime_t2 = par("nRentTime_t2");
    maxSubTime_t3 = par("maxSubTime_t3");
    maxSubscriptionTime = par("maxSubscriptionTime");

    activeCycles = par ("activeCycles");
    numberOfCycles = par ("numberOfCycles");
    durationOfCycle = par ("durationOfCycle");
    isolateCycle = par ("isolateCycle");
    cycleDistribution = &par ("cycleDistribution");

    numberOfCycles = par ("numberOfCycles");
    durationOfCycle = par ("durationOfCycle");
    isolateCycle = par ("isolateCycle");
    cycleDistribution = &par ("cycleDistribution");

    // Gates
    fromCloudProviderGate = gate ("fromCloudProvider");
    toCloudProviderGate = gate ("toCloudProvider");


    EV_INFO << "UserGeneratorBase::initialize - Generating users before simulation" << endl;

    groupOfUsers.clear();
    userInstances.clear();

    nUserInstancesFinished = 0;
    nUserIndex=0;

    // Create user instances
    generateUsersBeforeSimulationStarts();
    parseTraceFile();

    // Show generated users instances
    if (showUserInstances){
        EV_DEBUG << usersIstancesToString ();
    }

    EV_INFO << "UserGeneratorBase::initialize - Scheduling a message" << endl;

    // Start execution!
    cMessage *waitToExecuteMsg = new cMessage (Timer_WaitToExecute.c_str());
    scheduleAt (simTime()+SimTime(startDelay), waitToExecuteMsg);

    EV_INFO << "UserGeneratorBase::initialize - End" << endl;
}


cGate* UserGeneratorBase::getOutGate (cMessage *msg){

    cGate* nextGate;

           // Init...
           nextGate = nullptr;

           // If msg arrives from cloud provider
           if (msg->getArrivalGate()==fromCloudProviderGate){
               nextGate = toCloudProviderGate;
           }

       return nextGate;
}


void UserGeneratorBase::generateUsersBeforeSimulationStarts (){

    std::vector<CloudUser*>::iterator userTypeIterator;
    std::vector <CloudUserInstance*> userInstancesLocal;
    unsigned int currentUserNumber, currentUserInstance, totalUserInstance;
    CloudUserInstance* newUser;
    CloudUserInstance* pUser;
    int  nSize;

    // Init...
    userTypeIterator = userTypes.begin();
    currentUserNumber = totalUserInstance = 0;

    EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - Init" << endl;

    // Process each user type to generate the user instances
    while((userTypeIterator != userTypes.end()))
    {

        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 0" << endl;

        // New vector of user instances
        userInstancesLocal.clear();

        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 1" << endl;
        // Generate the corresponding instances of the current user type
        for (currentUserInstance = 0; currentUserInstance < (*userTypeIterator)->getNumInstances(); currentUserInstance++){

            EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 2" << endl;
            // Create a new user instance
            newUser = createCloudUserInstance (*userTypeIterator, totalUserInstance, currentUserNumber, currentUserInstance, (*userTypeIterator)->getNumInstances());

            EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 3" << endl;
            // Insert current user instance into the corresponding vector
            userInstancesLocal.push_back(newUser);
            userHashMap[newUser->getUserID()]=newUser;

            EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 4" << endl;
            //update user instance
            totalUserInstance++;

            userInstances.push_back(newUser);
        }


        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 5" << endl;
        // Insert current user instances in the global users vector
        groupOfUsers.push_back(userInstancesLocal);

        // Process next user type
        userTypeIterator++;
        currentUserNumber++;

        EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - 6" << endl;
    }

    generateUserArrivalTimes();

    EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - End" << endl;
}

void UserGeneratorBase::generateUserArrivalTimes() {
    SM_UserVM *userVm;
    CloudUserInstance *pUserInstance;
    CloudUserInstanceTrace *pUserInstanceTrace;
    SimTime lastTime;

    m_dInitSim = simTime()+SimTime(startDelay);

    lastTime = 0;

    if (shuffleUsers)
        generateShuffledUsers();

    for (int i = 0; i < userInstances.size(); i++)
      {
        // Get current user
        pUserInstance = userInstances.at(i);


        userVm = createVmRequest(pUserInstance);

        if (userVm != nullptr) pUserInstance->setRequestVmMsg(userVm);

        pUserInstanceTrace = dynamic_cast<CloudUserInstanceTrace*>(pUserInstance);
        if (pUserInstanceTrace == nullptr) {
            lastTime = getNextTime(pUserInstance, lastTime);
            // Set init and arrival time!
            pUserInstance->setInitTime(lastTime);
            pUserInstance->setArrival2Cloud(lastTime);
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
    else if (activeCycles) {
        next = SimTime(distribution->doubleValue()) + m_dInitSim;
    }
    else
      {
        int cycle = 0;
        next = SimTime(distribution->doubleValue());

        if (numberOfCycles > 1) {
            do {
                cycle = cycleDistribution->intValue();
            } while (cycle < 1 || cycle > numberOfCycles);
            cycle--;
        }

        SimTime offset = cycle * durationOfCycle;

        while (((isolateCycle || cycle == 0) && next < 0) ||
               ((isolateCycle || cycle == numberOfCycles - 1) && next > durationOfCycle) ||
               next + offset < 0 || next + offset > durationOfCycle * numberOfCycles) {
            next = SimTime(distribution->doubleValue());
        }

        next = m_dInitSim + next + offset;
      }

    return next;
}


SM_UserVM* UserGeneratorBase::createVmRequest(
        CloudUserInstance *pUserInstance) {
    int nVmIndex, nCollectionNumber, nInstances;
    double dRentTime;
    std::string userId, vmType, instanceId;
    SM_UserVM *pUserRet;
    VmInstance *pVmInstance;
    VmInstanceCollection *pVmCollection;

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - Init" << endl;

    pUserRet = nullptr;

    nVmIndex = 0;

    if (pUserInstance != nullptr) {
        pUserInstance->setRentTimes(maxStartTime_t1, nRentTime_t2,
                maxSubTime_t3, maxSubscriptionTime);
        nCollectionNumber = pUserInstance->getNumberVmCollections();
        userId = pUserInstance->getUserID();

        EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - UserId: " << userId
                        << " | maxStartTime_t1: " << maxStartTime_t1
                        << " | rentTime_t2: " << nRentTime_t2
                        << " | maxSubTime: " << maxSubTime_t3
                        << " | MaxSubscriptionTime:"
                        << maxSubscriptionTime << endl;

        if (nCollectionNumber > 0 && userId.size() > 0) {
            //Creation of the message
            pUserRet = createVmMessage();
            pUserRet->setUserID(userId.c_str());
            pUserRet->setIsResponse(false);
            pUserRet->setOperation(SM_VM_Req);

            //Get all the collections and all the instances!
            for (int i = 0; i < nCollectionNumber; i++) {
                pVmCollection = pUserInstance->getVmCollection(i);
                if (pVmCollection) {
                    nInstances = pVmCollection->getNumInstances();
                    dRentTime = pVmCollection->getRentTime() * 3600;
                    //Create a loop to insert all the instances.
                    vmType = pVmCollection->getVmType();
                    for (int j = 0; j < nInstances; j++) {
                        pVmInstance = pVmCollection->getVmInstance(j);
                        if (pVmInstance != NULL) {
                            instanceId = pVmInstance->getVmInstanceId();
                            pUserRet->createNewVmRequest(vmType, instanceId,
                                    maxStartTime_t1, dRentTime, maxSubTime_t3,
                                    maxSubscriptionTime);
                        }
                    }
                } else {
                    EV_TRACE
                                    << "WARNING! [UserGenerator] The VM collection is empty"
                                    << endl;
                    throw omnetpp::cRuntimeError(
                            "[UserGenerator] The VM collection is empty!");
                }
            }
        } else {
            EV_TRACE
                            << "WARNING! [UserGenerator] Collection or User-ID malformed"
                            << endl;
            throw omnetpp::cRuntimeError(
                    "[UserGenerator] Collection or User-ID malformed!");
        }
    } else {
        EV_INFO
                       << "UserGenerator::createNextVmRequest - The user instance is null"
                       << endl;
    }

    EV_TRACE << LogUtils::prettyFunc(__FILE__, __func__) << " - End" << endl;

    return pUserRet;
}

/**
 * Shuffle the list of users in order to reproduce the behaviour of the users in a real cloud environment.
 */
void UserGeneratorBase::generateShuffledUsers()
{
    int nRandom, nSize;

    srand((int)33); //TODO: semilla. Comprobar si con las semillas del .ini se puede omitir esta. AsÃ­ se controla mejor la aletoriedad solo desde el fichero .ini.
    EV_INFO << "UserGenerator::generateShuffledUsers - Init" << endl;

    nSize = userInstances.size();

    EV_INFO << "UserGenerator::generateShuffledUsers - instances size: "
                   << userInstances.size() << endl;
    //
    for (int i = 0; i < nSize; i++)
      {
        nRandom = rand() % nSize;
        std::iter_swap(userInstances.begin() + i,
                userInstances.begin() + nRandom);
      }

    EV_INFO << "UserGenerator::generateShuffledUsers - End" << endl;
}

string UserGeneratorBase::usersIstancesToString (){

    std::vector<std::vector <CloudUserInstance*>>::iterator globalIterator;
    std::vector <CloudUserInstance*>::iterator usersIterator;
    std::ostringstream info;


        // Main text
        info << std::endl << groupOfUsers.size() << " types of user instances in " << getFullPath() << endl << endl;

        // Init iterators
        globalIterator = groupOfUsers.begin();

        // For each entry in the users vector...
        while (globalIterator != groupOfUsers.end()){

            // Init iterator for the current user type
            usersIterator = (*globalIterator).begin();

            // Print user type
            if (usersIterator != (*globalIterator).end())
                info << "\tUser type:" << (*usersIterator)->getType() << "  -  #instances: " << (*globalIterator).size() << endl << endl;

            while (usersIterator != (*globalIterator).end()){

                info << "\t  + UserID: " << (*usersIterator)->toString(false, false) << endl;

                // Process next user instance
                usersIterator++;
            }

            // Process next user type
            globalIterator++;
        }

    return info.str();
}


CloudUserInstance* UserGeneratorBase::createCloudUserInstance(CloudUser *ptrUser, unsigned int  totalUserInstance, unsigned int  userNumber, int currentInstanceIndex, int totalUserInstances) {
    return new CloudUserInstance (ptrUser, totalUserInstance, userNumber, currentInstanceIndex, totalUserInstances);
}

SM_UserVM* UserGeneratorBase::createVmMessage() {
    return new SM_UserVM();
}

CloudUserInstance* UserGeneratorBase::createNewUserFromTrace(Job_t jobIn, int totalUserInstance, int nCurrentNumber, int nUserInstance, int nTotalInstances)
{
    CloudUserInstance* pNewUser;
    Application* pApp=findApplication("AppCPUIntensive");
    pNewUser = new CloudUserInstanceTrace(jobIn, totalUserInstance, nCurrentNumber, nUserInstance, nTotalInstances, pApp);
    return pNewUser;
}

CloudUser* UserGeneratorBase::createUserTraceType(){
    return new CloudUser("UserTrace", 0);
}

//Esto va directamente a otra clase
void UserGeneratorBase::parseTraceFile()
{
    SM_UserVM *userVm;
    string strPath;
    vector<Job_t> jobs;
    Job_t singleJob;
    CloudUserInstance* newUser;
    int nJobs, nTotalUserInstance;
    double dInitTime;
    CloudUser* currentUserObject;

    EV_DEBUG << "UserGeneratorBase::parseTraceFile - Init" << endl;

    //DONE: Esto se debe sacar de un parametro omnet, con = par.
    //strPath = "/home/pablo/applics/omnetpp-5.0/projects/SIMCAN-2.0/src/Management/traces/LPC.swf";
    parse_swf( &jobs, strUserTraceFilePath.c_str());
    nJobs = jobs.size();
    nTotalUserInstance = 0;

    //Show the jobs info
    if(nJobs > 0)
    {
        currentUserObject = createUserTraceType();
        userTypes.push_back(currentUserObject);


        EV_INFO << "UserGeneratorBase::parseTraceFile - Jobs successfully loaded: " << nJobs << endl;

        //TODO: Ojo con poner esto a fuego. Debug
        //for(int i=0;i<nJobs;i++)
        for(int i=0;i<nJobs;i++)
        {
            singleJob = jobs.at(i);
            EV_DEBUG << "UserGeneratorBase::parseTraceFile - Job#" << i << " [" << singleJob.size<< " CPU]" << "[" << singleJob.runtime/3600 << "H]" <<endl;

            if(singleJob.runtime > 0 && singleJob.size != -1 && singleJob.submit >= traceStartTime && singleJob.submit <= traceEndTime)
            {
                //Limit number of vms per user
                if (singleJob.size>userTraceMaxVms) singleJob.size = userTraceMaxVms;

                //TODO: Aqui tendría que ser necesario hacer un matching entre el tipo de maquina
                //y los cpus requeridos por el usuario

                //Para el paper de SUPE, tomamos la referencia de 1 cpu= 1 vmsmall
                //New user type with the job, and ids.
                dInitTime = singleJob.submit+startDelay-traceStartTime;
                newUser = createNewUserFromTrace(singleJob, nTotalUserInstance, nTotalUserInstance, nTotalUserInstance, nJobs);
                newUser->setArrival2Cloud(SimTime(dInitTime));
                newUser->setInitTime(SimTime(dInitTime));
                userVm = createVmRequest(newUser);
                //Create VM request
                if (userVm != nullptr) newUser->setRequestVmMsg(userVm);
                userHashMap[newUser->getUserID()]=newUser;
                EV_DEBUG << "UserGeneratorBase::parseTraceFile - User generated: "<< newUser->getUserID() << endl;
                //Aqui creo que falta algo, con respecto al otro generador
                userInstances.push_back(newUser);

                nTotalUserInstance++;

                if(EV_TRACE)
                    singleJob.write(std::cout,1);
            }
        }
    }
    EV_DEBUG << "UserGeneratorBase::parseTraceFile - End" << endl;
}

//------------------------------------------------------------------------------
// read jobs from the given SWFfile into the jobs vector, while copying all
// non-job lines to the standard output (this is the header).
//
// only "sane" jobs are read, the rest are filtered out. a sane job is a jon
// with a positive and and a nonnegative runtime.
//------------------------------------------------------------------------------
static void parse_swf(vector<Job_t> *jobs, const char* swf_file)
{
    // 1- open the SWFfile
    std::ifstream swf(swf_file);
    if(swf)
    {
        // 2- ready...
            jobs->clear();
            jobs->reserve(100000);

            // 3- go!
            int insane=0;
            for(string line; ! getline(swf,line).eof(); ) {
            Job_t job;
            if( job.read( line.c_str() ) )
                if( job.runtime >= 0 && job.size > 0 ) // sane job
                jobs->push_back( job );
                else
                insane++;
            else
                std::cout << line << "\n";
            }

            // 4- warn if filtered jobs
            if( insane > 0 )
            fprintf(stderr,
                "#\n"
                "# WARNING: %d jobs were filtered out (size<1 or runtime<0)\n",
                insane);
    }
}
