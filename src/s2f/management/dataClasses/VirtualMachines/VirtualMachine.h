#ifndef VIRTUALMACHINE_H_
#define VIRTUALMACHINE_H_

#include "s2f/core/include/SIMCAN_types.h"

/**
 * Class that represents the main features of a VM in the cloud.
 *
 */
class VirtualMachine
{
private:
    std::string type; // Name of the VM in the SIMCAN repository
    double cost;      // Cost (per hour)
    int numCores;     // Number of cores
    double scu;       // Scientific Computing Units
    double diskGB;    // Disk space (in GB)
    double memoryGB;  // Memory space (in GB)

public:
    /**
     * Constructor that initializes this VM with the corresponding parameters.
     *
     * @param type Type of the VM.
     * @param cost Cost per hour.
     * @param numCores Number of cores.
     * @param scu Computing units.
     * @param diskGB Amount of disk space (in GB)
     * @param memoryGB Amount of RAM memory (in GB)
     */
    VirtualMachine(std::string type, double cost, int numCores, double scu, double diskGB, double memoryGB);

    /**
     * Gets the cost of this VM.
     *
     * @return Cost of this VM.
     */
    double getCost() const { return cost; }

    /**
     * Sets a new cost for this VM.
     *
     * @param cost New cost for this VM
     */
    void setCost(double cost) { this->cost = cost; }

    /**
     * Gets the amount of disk space of this VM.
     *
     * @return Amount of disk space of this VM
     */
    double getDiskGb() const { return diskGB; }

    /**
     * Sets a new amount of disk space for this VM.
     *
     * @param diskGb New amount of disk space for this VM
     */
    void setDiskGb(double diskGb) { diskGB = diskGb; }

    /**
     * Gets the amount of RAM memory (in GB) of this VM.
     *
     * @return Amount of RAM memory (in GB) of this VM
     */
    double getMemoryGb() const { return memoryGB; }

    /**
     * Sets a new amount of RAM memory (in GB) for this VM.
     *
     * @param memoryGb New amount of RAM memory (in GB) for this VM
     */
    void setMemoryGb(double memoryGb) { memoryGB = memoryGb; }

    /**
     * Gets the number of CPU cores of this VM.
     *
     * @return Number of CPU cores of this VM.
     */
    int getNumCores() const { return numCores; }

    /**
     * Sets a new number of CPU cores for this VM.
     *
     * @param numCores New number of CPU cores for this VM.
     */
    void setNumCores(int numCores) { this->numCores = numCores; }

    /**
     * Gets the computing units of this VM.
     *
     * @return Computing units of this VM.
     */
    double getScu() const { return scu; }

    /**
     * Sets a new number of computing units for this VM.
     *
     * @param scu New number of computing units for this VM.
     */
    void setScu(double scu) { this->scu = scu; }

    /**
     * Gets the type of this VM.
     *
     * @return Type of this VM
     */
    const std::string &getType() const { return type; }

    /**
     * Sets a new type for this VM.
     *
     * @param type new type for this VM.
     */
    void setType(const std::string &type) { this->type = type; }

    /**
     * @brief Outputs an object instance into a stream
     *
     * @param os Stream to be written
     * @param obj VirtualMachine object to be read
     * @return std::ostream& Same stream as the one inserted
     */
    friend std::ostream &operator<<(std::ostream &os, const VirtualMachine &obj);
};

#endif /* VIRTUALMACHINE_H_ */
