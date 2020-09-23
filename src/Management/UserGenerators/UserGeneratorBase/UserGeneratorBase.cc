#include "UserGeneratorBase.h"

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

    EV_DEBUG << "UserGeneratorBase::generateUsersBeforeSimulationStarts - End" << endl;
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
