#ifndef NODEUSER_H_
#define NODEUSER_H_

#include <string>
#include <vector>
#include <omnetpp.h>
#include "NodeResourceRequest.h"
#include "Management/dataClasses/NodeInfo.h"
using namespace omnetpp;

/**
 * This class represents a reduced version of a node. It contains the essential information about the resources that are used to allocate VMs.
 */
class NodeResourceInfo: public NodeInfo{

    private:
        /* Ip of the node*/
        std::string ip;

        /** Node type*/
        std::string nodeType;

        /** This flag denotes if it is an storage node*/
        //bool bStorage;

        /** The number of datacenter where this node is located. This information is used for finding the resources in a fast way.*/
        int nDataCenter;

        //Data related with the user general resource information
        /** Total number of CPUs of the node*/
        //unsigned int nTotalCPUs;

        /** Total number of RAM memory of the node*/
        //unsigned int nTotalMemory;

        /** CPU speed*/
        //unsigned int nCpuSpeed;

        /** Storage size of the disk*/
        //double dTotalDiskGB;

        //Available data
        /** Number of available CPUs of the node*/
        unsigned int nAvailableCPUs;

        /** Number of available RAM memory of the node*/
        unsigned int nAvailableMemory;

        /** Number of available storage size of the node*/
        double dAvailableDiskGB;

        /** Number of users that have allocated resources in this node*/
        int nTotalUsers;

        /** Map that contains both the users and their allocated resources*/
        std::map<std::string, NodeResourceRequest*> userRqMap;

    public:
        /**
         * Constructor.
         */
        NodeResourceInfo();

        /**
         * Destructor.
         */
        ~NodeResourceInfo();

        /**
         * Check if the user request fits in the current Node.
         * @param pUserRq The resources requested by the user.
         * @return TRUE if the request fits in the node, FALSE i.o.c.
         */
        bool hasFit(NodeResourceRequest* pUserRq);

        /**
         * Insert the resources requested by the user in this node.
         * @param pUserRq The resources requested by the user.
         * @return TRUE if the request has been allocated, FALSE i.o.c.
         */
        bool insertUserRequest(NodeResourceRequest* pUserRq);

        /**
         * Free the resources of an specific request.
         * @param pUserRq The resources to be released.
         */
        void freeResources(NodeResourceRequest* pUser);

        /**
         * Free the resources of a VM, given an id.
         * @param strVmId Identifier of the VM to release
         */
        void freeVmById(std::string strVmId);

        /**
         * Searches an user given its identifier.
         * @param strUserName Identifier of the user
         * @return TRUE if the user has been found, FALSE i.o.c.
         */
        bool searchUser(std::string strUserName);

        /**
         * Print the users that have resources allocated in this node.
         */
        void printNodeUsers();

        /**
         *  Print a summary about the available resources of the node.
         */
        void printAvailableResources();

        /**
         * A simplified version of the insertion method. Used in first instance to carry out the debuging phase
         * @param pUserRq The resources to be released.
         * @return TRUE if the request has been allocated, FALSE i.o.c.
         */
        bool simpleInserUserRequest(NodeResourceRequest* pUser);

        /**
         * Get the total number of cpus allocated by a VM.
         * @param strVmId Identifier of the VM.
         * @return
         */
        unsigned int getTotalCpusById(std::string strVmId);

        double getAvailableDiskGb() const {return dAvailableDiskGB;}
        void setAvailableDiskGb(double availableDiskGb) {dAvailableDiskGB = availableDiskGb;}
        const std::string& getIp() const {return ip;}
        void setIp(const std::string& ip) {this->ip = ip;}
        unsigned int getAvailableCpUs() const {return nAvailableCPUs;}
        void setAvailableCpUs(unsigned int availableCpUs) {nAvailableCPUs = availableCpUs;}
        unsigned int getAvailableMemory() const {return nAvailableMemory;}
        void setAvailableMemory(unsigned int availableMemory) {nAvailableMemory = availableMemory;}
        const std::string& getNodeType() const {return nodeType;}
        void setNodeType(const std::string& nodeType) {this->nodeType = nodeType;}
        int getTotalUsers() const {return nTotalUsers;}
        void setTotalUsers(int totalUsers) {nTotalUsers = totalUsers;}
        int getDataCenter() const {return nDataCenter;}
        void setDataCenter(int dataCenter) {nDataCenter = dataCenter;}
};

#endif /* NODEUSER_H_ */
