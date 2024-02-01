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
class Node
{

private:
    std::string ip;           //!< IP address
    double remainingMemoryGB; //!< Remaining memory space (in GB)
    double remainingDiskGB;   //!< Remaining disk space (in GB)
    tCpuState *cpuState;      //!< Array that shows the state of each CPU core
    int numCpus;              //!< Number of CPU cores
    int numAvailableCpus;     //!< Number of available CPUs
    bool racked;              //!< Is this node allocated in a rack?

public:
    /**
     * Constructor.
     *
     * @param remainingMemoryGB Amount of unused RAM memory (in GB).
     * @param remainingDiskGB Amount of unused disk space (in GB).
     * @param numCpus Number of CPU cores.
     * @param racked If set to \a true, this node is allocated in a rack. In other case, this node represents a basic single node.
     */
    Node(double remainingMemoryGB, double remainingDiskGB, int numCpus, bool racked);

    /**
     * @brief Gets the Ip
     * @return const std::string& The selected ip
     */
    const std::string &getIp() const { return ip; }

    /**
     * @brief Sets the Ip
     * @param ip Ip of the node
     */
    void setIp(const std::string &ip) { this->ip = ip; }

    /**
     * @brief Get the number of available Cpus
     * @return int The number of cpus
     */
    int getNumAvailableCpus() const { return numAvailableCpus; }

    /**
     * @brief Set the number of available Cpus
     * @param numAvailableCpus The number of cpus
     */
    void setNumAvailableCpus(int numAvailableCpus) { this->numAvailableCpus = numAvailableCpus; }

    /**
     * Gets the number of CPU cores.
     * @return Number of CPU cores
     */
    int getNumCpus() const { return numCpus; }

    /**
     * Sets a new number of CPU cores.
     * @param numCpus New number of CPU cores.
     */
    void setNumCpus(int numCpus) {}

    /**
     * Gets if this node is allocated in a rack.
     * @return \a True if this node is allocated in a rack or \false if this node is a single node.
     */
    bool isRacked() const { return racked; }

    /**
     * Sets whether this node is allocated in a rack or not.
     * @param racked It set to \a true, this node is allocated in a rack. In other case, this node represents a basic single node.
     */
    void setRacked(bool racked) { this->racked = racked; }

    /**
     * Gets the unused disk space (in GB).
     * @return Unused disk space (in GB).
     */
    double getRemainingDiskGb() const { return remainingDiskGB; }

    /**
     * Sets a new amount of unused disk space (in GB).
     * @param remainingDiskGb New amount of unused disk space (in GB).
     */
    void setRemaininDiskGb(double remainingDiskGb) { this->remainingDiskGB = remainingDiskGB; }

    /**
     * Gets the unused RAM memory (in GB).
     * @return Unused RAM memory (in GB).
     */
    double getRemainingMemoryGb() const { return remainingMemoryGB; }

    /**
     * Sets a new amount of unused RAM memory (in GB).
     * @param remainingMemoryGb New amount of unused RAM memory (in GB).
     */
    void setRemainingMemoryGb(double remainingMemoryGb) { this->remainingMemoryGB = remainingMemoryGB; }

    /**
     * Sets a new state for the CPU core at <b>cpuIndex</b> position. If the \a cpuIndex position does not exist, an error is raised.
     *
     * @param newState New state for the CPU core.
     * @param cpuIndex Position of the CPU core at <b>cpuState</b> vector.
     */
    void setState(tCpuState newState, int cpuIndex);

    /**
     * Gets the state of the CPU core at <b>cpuIndex</b> position. If the \a cpuIndex position does not exist, an error is raised.
     * @param cpuIndex Position of the CPU core at <b>cpuState</b> vector.
     */
    tCpuState getState(int cpuIndex);

    friend std::ostream &operator<<(std::ostream &os, const Node &obj);
};

#endif /* NODE_H_ */
