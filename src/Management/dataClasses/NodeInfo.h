#ifndef NODEINFO_H_
#define NODEINFO_H_

#include <string>
#include "Core/include/SIMCAN_types.h"

/**
 *
 * Class that contains the main features of a node.
 *
 * This class represents the configuration of a node in the SIMCAN repository.
 *
 */
class NodeInfo{

    private:

        /** Name of this Node in the SIMCAN repository */
        std::string name;

        /** Is this node a storage node? */
        bool storage;

        /** Disk space (in GB) */
        double totalDiskGB;

        /** RAM memory (in GB) */
        double totalMemoryGB;

        /** Number of CPU cores */
        int numCPUs;

        /** CPU speed (in MIPS) */
        int cpuSpeed;

    public:


        /**
         * Constructor.
         *
         * @param name Name of the node in the SIMCAN repository.
         * @param storage If this parameter is set to \a true, this node is a storage node. In other case, this node represents a computing node.
         * @param totalDiskGB Disk space (in GB).
         * @param totalMemoryGB RAM memory (in GB).
         * @param numCpus Number of CPU cores.
         * @param cpuSpeed Processing speed (in MIPS).
         */
        NodeInfo(std::string name, bool storage, double totalDiskGB, double totalMemoryGB, int numCpus, int cpuSpeed);

        /**
         * Gets the processing speed.
         *
         * @return Processing speed
         */
        int getCpuSpeed() const;

        /**
         * Sets a new processing speed.
         *
         * @param cpuSpeed New processing speed (in MIPS).
         */
        void setCpuSpeed(int cpuSpeed);

        /**
         * Gets the name of this node.
         *
         * @return Name of this node.
         */
        const std::string& getName() const;

        /**
         * Sets a new name for this node.
         *
         * @param name New name for this node.
         */
        void setName(const std::string& name);

        /**
         * Gets the number of CPU cores.
         *
         * @return Number of CPU cores.
         */
        int getNumCpUs() const;

        /**
         * Sets a new number of CPU cores for this node.
         *
         * @param numCpUs New number of CPU cores for this node
         */
        void setNumCpUs(int numCpUs);

        /**
         * Gets the type of this node.
         *
         * @return If this node is a storage node, \a true is returned. If this node is a computing node, \a false is returned
         */
        bool isStorage() const;

        /**
         * Sets the type of this node.
         *
         * @param storage If this parameter is set to \a true, this node is a storage node. In other case, this node represents a computing node.
         */
        void setStorage(bool storage);

        /**
         * Gets the disk size (in GB) of this node.
         *
         * @return Disk size (in GB) of this node
         */
        double getTotalDiskGb() const;

        /**
         * Sets a new disk size (in GB) for this node.
         *
         * @param totalDiskGb New disk size (in GB) for this node.
         */
        void setTotalDiskGb(double totalDiskGb);

        /**
         * Gets the RAM memory (in GB) of this node.
         *
         * @return RAM memory (in GB) of this node.
         */
        double getTotalMemoryGb() const;

        /**
         * Sets a new amount of RAM memory (in GB) for this node.
         *
         * @param totalMemoryGb new amount of RAM memory (in GB) for this node.
         */
        void setTotalMemoryGb(double totalMemoryGb);

        /**
         * Converts the information of this node to string format. Usually, this method is invoked for debugging/logging purposes.
         *
         * @return String containing the configuration of this node.
         */
        std::string toString ();
};

#endif /* NodeInfo_H_ */
