#ifndef VMINSTANCECOLLECTION_H_
#define VMINSTANCECOLLECTION_H_

#include "VmInstance.h"

/**
 *
 * Class that represents a collection of VM instances to be executed by a user, where each instance represents a copy of the same VM.
 *
 * Each instance is differentiated by its <i>vmInstanceID</i>.
 *
 */
class VmInstanceCollection{

    private:

        /** Pointer to the object that contains the features of this VM */
        VirtualMachine* vmBase;

        /** Collection of VM instances to be instantiated */
        std::vector<VmInstance*> vmInstances;

        /** Number of renting hours for each VM instance*/
        int nRentTime;

    public:

        /**
         * Constructor.
         *
         * Sets the pointer to the object that contains the features of this VM and creates <i>numInstances</i> VM instances.
         *
         * @param vmPtr Pointer to the object that contains the features of this VM.
         * @param userID User that contains this collection of VMs.
         * @param numInstances Number of VM instances to be created.
         */
        VmInstanceCollection(VirtualMachine* vmPtr, std::string userID, int numInstances, int nRentTime, int total, int offset);

        /**
         * Destructor
         */
        virtual ~VmInstanceCollection();

        /**
         * Gets the pointer to the object that contains the features of this VM.
         *
         * @return Pointer to the object that contains the features of this VM.
         */
        VirtualMachine* getVirtualMachineBase ();

        /**
         * Gets the number of instances.
         *
         * @return Number of instances.
         */
        int getNumInstances ();

        /**
         * Parses of this collection of VM instances into string format.
         *
         * @param includeVmFeatures If set to \a true, the generated string will contain the features of each VM in the <b>virtualMachines</b> vector.
         * @return String containing this collection of VM instances into string format.
         */
        std::string toString (bool includeVmFeatures);

        /*
         * Returns the type of the base VM
         */
        std::string getVmType();

        /**
         *
         */
        VmInstance* getVmInstance(int nIndex);

        int getRentTime(){return nRentTime;};
        void setRentTime(int nRentTime){this->nRentTime=nRentTime;}
    private:

        /**
         * Generates <i>numInstances</i> VM instances and insert them into the <b>vmInstances</b> vector.
         *
         * @param userID User that contains this collection of VMs.
         * @param numInstances Number of VM instances to be created.
         */
         void generateInstances (std::string userID, int numInstances, int total, int offset);
};

#endif /* APPINSTANCECOLLECTION_H_ */
