#ifndef NODE_H_
#define NODE_H_

#include "Core/include/SIMCAN_types.h"

/**
 *
 * Class that monitorizes the current status of a node.
 *
 * This class represents the used and unused features of a node.
 *
 */
class Node{

    private:

        /** IP address */
        std::string ip;

        /** Remaining memory space (in GB) */
        double remainingMemoryGB;

        /** Remaining disk space (in GB) */
        double remaininDiskGB;

        /** Array that shows the state of each CPU core */
        tCpuState* cpuState;

        /** Number of CPU cores */
        int numCpus;

        /** Number of available CPUs */
        int numAvailableCpus;

        /** Is this node allocated in a rack? */
        bool racked;

    public:

        /**
         * Constructor.
         *
         * @param remainingMemoryGB Amount of unused RAM memory (in GB).
         * @param remaininDiskGB Amount of unused disk space (in GB).
         * @param numCpus Number of CPU cores.
         * @param racked If set to \a true, this node is allocated in a rack. In other case, this node represents a basic single node.
         */
        Node(double remainingMemoryGB, double remaininDiskGB, int numCpus, bool racked);

        /**
         *
         */
        const std::string& getIp() const;

        /**
         *
         */
        void setIp(const std::string& ip);

        /**
         *
         */
        int getNumAvailableCpus() const;

        /**
         *
         */
        void setNumAvailableCpus(int numAvailableCpus);

        /**
         * Gets the number of CPU cores.
         *
         * @return Number of CPU cores
         */
        int getNumCpus() const;

        /**
         * Sets a new number of CPU cores.
         *
         * @param numCpus New number of CPU cores.
         */
        void setNumCpus(int numCpus);

        /**
         * Gets if this node is allocated in a rack.
         *
         * @return \a True if this node is allocated in a rack or \false if this node is a single node.
         */
        bool isRacked() const;

        /**
         * Sets whether this node is allocated in a rack or not.
         *
         * @param racked It set to \a true, this node is allocated in a rack. In other case, this node represents a basic single node.
         */
        void setRacked(bool racked);

        /**
         * Gets the unused disk space (in GB).
         *
         * @return Unused disk space (in GB).
         */
        double getRemaininDiskGb() const;

        /**
         * Sets a new amount of unused disk space (in GB).
         *
         * @param remaininDiskGb New amount of unused disk space (in GB).
         */
        void setRemaininDiskGb(double remaininDiskGb);

        /**
         * Gets the unused RAM memory (in GB).
         *
         * @return Unused RAM memory (in GB).
         */
        double getRemainingMemoryGb() const;

        /**
         * Sets a new amount of unused RAM memory (in GB).
         *
         * @param remainingMemoryGb New amount of unused RAM memory (in GB).
         */
        void setRemainingMemoryGb(double remainingMemoryGb);

        /**
         * Sets a new state for the CPU core at <b>cpuIndex</b> position. If the \a cpuIndex position does not exist, an error is raised.
         *
         * @param newState New state for the CPU core.
         * @param cpuIndex Position of the CPU core at <b>cpuState</b> vector.
         */
        void setState (tCpuState newState, int cpuIndex);

        /**
         * Gets the state of the CPU core at <b>cpuIndex</b> position. If the \a cpuIndex position does not exist, an error is raised.
         *
         * @param cpuIndex Position of the CPU core at <b>cpuState</b> vector.
         */
        tCpuState getState (int cpuIndex);

        /**
         *
         */
        //NodeResourceUser* getUserInfoByIndex (unsigned int index);

        /**
         *
         */
        //NodeResourceUser* getUserInfoByName (std::string userName);


        /**
         * Converts the parsed node to string format. Usually, this method is invoked for debugging/logging purposes.
         *
         * @return String containing the status of this node.
         */
        std::string toString ();
};

#endif /* Node_H_ */
