#include "NodeResourceInfo.h"

NodeResourceInfo::NodeResourceInfo():NodeInfo("",false,0,0,0,0){

    ip = "";
    nodeType = "";

    nTotalUsers = nAvailableCPUs = nAvailableMemory = 0;
    userRqMap.clear();
}

NodeResourceInfo::~NodeResourceInfo() {

}

bool NodeResourceInfo::hasFit(NodeResourceRequest* pUser)
{
    bool bRet;

    bRet=false;
    if(pUser != NULL)
    {
        bRet=true;
        if(pUser->getTotalCpus() > this->nAvailableCPUs)
        {
            bRet = false;
            EV_TRACE << "Full cores list!!(max: "<<this->getNumCpUs() << "): " << pUser->getTotalCpus() << " vs "<< this->nAvailableCPUs << endl;

        }

        if(bRet && pUser->getTotalMemory() > this->nAvailableMemory)
        {
            bRet = false;
            EV_TRACE << "Full memory list!!(max: "<< this->getTotalMemoryGb() << "): " << pUser->getTotalMemory() << " vs "<< this->nAvailableMemory << endl;
        }
    }

    return bRet;
}
bool NodeResourceInfo::simpleInserUserRequest(NodeResourceRequest* pUser)
{
    bool bRet = false;

    nAvailableCPUs=nAvailableCPUs-pUser->getTotalCpus();
    nAvailableMemory=nAvailableMemory-pUser->getTotalMemory();
    userRqMap[pUser->getVmId()]=pUser;

    return bRet;
}
bool NodeResourceInfo::insertUserRequest(NodeResourceRequest* pUser)
{
    bool bRet;
    int nCpus, nMemory;
    std::string strVmId;
    bRet = false;

    EV_DEBUG << "DataCenter_CpuType::insertUserRequest - Init" << endl;
    if(hasFit(pUser))
    {
        strVmId = pUser->getVmId();
        nCpus = pUser->getTotalCpus();
        nMemory = pUser->getTotalMemory();

        //Trying to avoid duplicated requests
        if(!searchUser(strVmId))
        {
            this->userRqMap[pUser->getVmId()]=pUser;
        }
        nTotalUsers++;

        //Update resources
        this->nAvailableCPUs=this->nAvailableCPUs-nCpus;
        this->nAvailableMemory=this->nAvailableMemory-nMemory;
        EV_DEBUG << "NodeResourceInfo::insertUserRequest - Required cores: " << nCpus<< " | Available: " << nAvailableCPUs << " vs Total: " << getNumCpUs()<<endl;
    }
    return bRet;
}
void NodeResourceInfo::freeResources(NodeResourceRequest* pUser)
{
    int nCpus, nMemory;

    EV_TRACE << "NodeResourceInfo::freeResources - Init" << endl;

    if(pUser != NULL)
    {
        nCpus = pUser->getTotalCpus();
        nMemory = pUser->getTotalMemory();
        EV_TRACE << "NodeResourceInfo::freeResources - freeing the resources of the user: " << pUser->getUserName() << " | CPUs: "<< nCpus << " | MEM: " << nMemory<<endl;

        nAvailableCPUs+=nCpus;
        nAvailableMemory+=nMemory;
        nTotalUsers--;
    }
    EV_TRACE << "NodeResourceInfo::freeResources - End" << endl;
}
void NodeResourceInfo::freeVmById(std::string strVmId)
{
    bool bFound;
    NodeResourceRequest* pUser;
    std::map<std::string, NodeResourceRequest*>::iterator it;

    EV_TRACE << "NodeResourceInfo::freeVmById - Init" << endl;

    bFound =  false;
    for (it=userRqMap.begin(); it!=userRqMap.end() && !bFound; )
    {
        pUser = it->second;
        if(strVmId.compare(pUser->getVmId())>=0)
        {
            EV_DEBUG << "NodeResourceInfo::freeVmById - " << strVmId << endl;
            freeResources(pUser);
            userRqMap.erase(it);
            bFound=true;
        } else {
            ++it;
        }
    }

    if(!bFound)
    {
        EV_INFO << "NodeResourceInfo::freeVmById - WARNING! The VM has not been found: " << strVmId << endl;
        printNodeUsers();
    }
    else
        EV_TRACE << "NodeResourceInfo::freeVmById - VM has been found, and deleted sucessfully: " << strVmId << endl;

}
bool NodeResourceInfo::searchUser(std::string strUserName)
{
    bool bRet = false;
    NodeResourceRequest pUser;

    EV_TRACE << "NodeResourceInfo::searchUser - Init" << endl;
    EV_TRACE << "NodeResourceInfo::searchUser - Size: " << userRqMap.size() << endl;
    if(userRqMap.find(strUserName) != userRqMap.end())
    {
        bRet = true;
        EV_DEBUG << "NodeResourceInfo::searchUser - Userfound" << endl;
    }
    else
        EV_TRACE << "NodeResourceInfo::searchUser - User NOT found" << endl;

    EV_TRACE << "NodeResourceInfo::searchUser - End" << endl;
    return bRet;
}
void NodeResourceInfo::printNodeUsers()
{
    int nIndex;
    NodeResourceRequest* pUser;
    std::map<std::string, NodeResourceRequest*>::iterator it;

    nIndex=0;

    EV_TRACE << "NodeResourceInfo::printNodeContent - Init" << endl;

    EV_DEBUG << "Printing the content of the node, size: " << this->userRqMap.size() << endl;
    for (it=userRqMap.begin(); it!=userRqMap.end(); ++it)
    {
        pUser = it->second;

        if(pUser != nullptr)
            EV_DEBUG << "NodeResourceInfo::printNodeContent -"<< ip << "- VM- "<< nIndex << pUser->getVmId() << endl;
        else
            EV_INFO << "NodeResourceInfo::printNodeContent - null user"<< endl;

        nIndex++;
    }
    EV_TRACE << "NodeResourceInfo::printNodeContent - End" << endl;
}
void NodeResourceInfo::printAvailableResources()
{
    EV_DEBUG << "NodeResourceInfo::printAvailableResources - Ip: "<< ip << " | Cores: - "<< nAvailableCPUs << " | Mem: "<< nAvailableMemory << endl;
}
unsigned int NodeResourceInfo::getTotalCpusById(std::string strVmId)
{
    int nRet;
    bool bFound;
    NodeResourceRequest* pUser;
    std::map<std::string, NodeResourceRequest*>::iterator it;

    EV_TRACE << "NodeResourceInfo::getTotalCpusById - Init" << endl;

    nRet=0;
    bFound =  false;
    for (it=userRqMap.begin(); it!=userRqMap.end() && !bFound; ++it)
    {
       pUser = it->second;
       if(strVmId.compare(pUser->getVmId())>=0)
       {
           nRet = pUser->getTotalCpus();
           bFound=true;
       }
    }

    if(!bFound)
    {
        EV_TRACE << "NodeResourceInfo::getTotalCpusById - The VM has not been found: " << strVmId << endl;
       printNodeUsers();
    }
    else
       EV_DEBUG << "NodeResourceInfo::getTotalCpusById - VM has been found, total size: " << nRet << endl;

    EV_TRACE << "NodeResourceInfo::getTotalCpusById - End" << endl;

    return nRet;
}
/*
void NodeResourceInfo::freeUser(std::string strUserName)
{
    NodeResourceUser pUser;

    pUser = userRqMap.at(strUserName);

    freeResources(&pUser);
    userRqMap.erase(strUserName);
}

void NodeResourceInfo::freeVmFromUser(std::string strUserName, std::string strType)
{
    bool bFound;
    NodeResourceUser pUser;
    std::map<std::string, NodeResourceUser>::iterator it;

    bFound =  false;
    for (it=userRqMap.begin(); it!=userRqMap.end() && !bFound; ++it)
    {
        // iterator->first = key
        // iterator->second = value
        pUser = it->second;
        if(strType.compare(pUser.getNodeType())>=0)
        {
            freeResources(&pUser);
            userRqMap.erase(it);
            bFound=true;
        }
    }
}
*/
