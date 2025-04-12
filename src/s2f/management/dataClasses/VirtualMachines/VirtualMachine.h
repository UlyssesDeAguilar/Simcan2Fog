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
    std::string type;   //!< Name of the VM in the SIMCAN repository
    uint32_t numCores;  //!< Number of cores
    uint32_t diskMiB;   //!< Disk space (in GB)
    uint32_t memoryMiB; //!< Memory space (in GB)
    double cost;        //!< Cost (per hour)
    double scu;         //!< Scientific Computing Units

public:
    /**
     * Constructor that initializes this VM with the corresponding parameters.
     *
     * @param type Type of the VM.
     * @param cost Cost per hour.
     * @param numCores Number of cores.
     * @param scu Computing units.
     * @param diskMiB Amount of disk space (in MiB)
     * @param memoryMiB Amount of RAM memory (in MiB)
     */
    VirtualMachine(const char *type,
                   uint32_t numCores,
                   uint32_t diskMiB,
                   uint32_t memoryMiB,
                   double cost,
                   double scu) : type(type), numCores(numCores), diskMiB(diskMiB), memoryMiB(memoryMiB), cost(cost), scu(scu) {};

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
     * @return Amount of disk space of this VM (in MiB)
     */
    uint32_t getDiskMiB() const { return diskMiB; }

    /**
     * Sets a new amount of disk space for this VM.
     *
     * @param diskGb New amount of disk space for this VM
     */
    void setDiskMiB(uint32_t diskMiB) { this->diskMiB = diskMiB; }

    /**
     * Gets the amount of RAM memory (in GB) of this VM.
     *
     * @return Amount of RAM memory (in GB) of this VM
     */
    uint32_t getMemoryMiB() const { return memoryMiB; }

    /**
     * Sets a new amount of RAM memory (in GB) for this VM.
     *
     * @param memoryGb New amount of RAM memory (in GB) for this VM
     */
    void setMemoryGb(uint32_t memoryMiB) { this->memoryMiB = memoryMiB; }

    /**
     * Gets the number of CPU cores of this VM.
     *
     * @return Number of CPU cores of this VM.
     */
    uint32_t getNumCores() const { return numCores; }

    /**
     * Sets a new number of CPU cores for this VM.
     *
     * @param numCores New number of CPU cores for this VM.
     */
    void setNumCores(uint32_t numCores) { this->numCores = numCores; }

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
