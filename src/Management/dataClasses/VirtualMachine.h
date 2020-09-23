#ifndef VIRTUALMACHINE_H_
#define VIRTUALMACHINE_H_

#include "Core/include/SIMCAN_types.h"

/**
 * Class that represents the main features of a VM in the cloud.
 *
 */
class VirtualMachine {

    private:

        /** Name of the VM in the SIMCAN repository */
        std::string type;

        /** Cost (per hour) */
        double cost;

        /** Number of cores */
        int numCores;

        /** Computing units */
        double scu;

        /** Disk space (in GB) */
        double diskGB;

        /** Memory space (in GB) */
        double memoryGB;


    public:

        /**
         * Constructor that initializes this VM.
         */
        VirtualMachine();

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
         * Destructor.
         */
        virtual ~VirtualMachine();

        /**
         * Gets the cost of this VM.
         *
         * @return Cost of this VM.
         */
        double getCost() const;

        /**
         * Sets a new cost for this VM.
         *
         * @param cost New cost for this VM
         */
        void setCost(double cost);

        /**
         * Gets the amount of disk space of this VM.
         *
         * @return Amount of disk space of this VM
         */
        double getDiskGb() const;

        /**
         * Sets a new amount of disk space for this VM.
         *
         * @param diskGb New amount of disk space for this VM
         */
        void setDiskGb(double diskGb);

        /**
         * Gets the amount of RAM memory (in GB) of this VM.
         *
         * @return Amount of RAM memory (in GB) of this VM
         */
        double getMemoryGb() const;

        /**
         * Sets a new amount of RAM memory (in GB) for this VM.
         *
         * @param memoryGb New amount of RAM memory (in GB) for this VM
         */
        void setMemoryGb(double memoryGb);

        /**
         * Gets the number of CPU cores of this VM.
         *
         * @return Number of CPU cores of this VM.
         */
        int getNumCores() const;

        /**
         * Sets a new number of CPU cores for this VM.
         *
         * @param numCores New number of CPU cores for this VM.
         */
        void setNumCores(int numCores);

        /**
         * Gets the computing units of this VM.
         *
         * @return Computing units of this VM.
         */
        double getScu() const;

        /**
         * Sets a new number of computing units for this VM.
         *
         * @param scu New number of computing units for this VM.
         */
        void setScu(double scu);

        /**
         * Gets the type of this VM.
         *
         * @return Type of this VM
         */
        const std::string& getType() const;

        /**
         * Sets a new type for this VM.
         *
         * @param type new type for this VM.
         */
        void setType(const std::string& type);

        /**
         * Converts the features of this VM into a string.
         *
         * @return A string containing the features of this VM.
         */
        std::string featuresToString();
};

#endif /* VIRTUALMACHINE_H_ */
