#ifndef VMINSTANCECOLLECTION_H_
#define VMINSTANCECOLLECTION_H_

#include "VmInstance.h"

/**
 * @brief Class that represents a collection of VM instances to be executed by a user, where each instance represents a copy of the same VM.
 * @details Each instance is differentiated by its <i>vmInstanceID</i>.
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar Gudmundsson
 */
class VmInstanceCollection
{
private:
    VirtualMachine *vmBase;                // Pointer to the object that contains the features of this VM
    std::vector<VmInstance *> vmInstances; // Collection of VM instances to be instantiated
    int nRentTime;                         // Number of renting hours for each VM instance

    /**
     * Generates <i>numInstances</i> VM instances and insert them into the <b>vmInstances</b> vector.
     *
     * @param userID User that contains this collection of VMs.
     * @param numInstances Number of VM instances to be created.
     */
    void generateInstances(std::string userID, int numInstances, int total, int offset);

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
    VmInstanceCollection(VirtualMachine *vmPtr, std::string userID, int numInstances, int nRentTime, int total, int offset);

    /**
     * Destructor
     */
    virtual ~VmInstanceCollection();

    /**
     * Gets the pointer to the object that contains the features of this VM.
     *
     * @return Pointer to the object that contains the features of this VM.
     */
    VirtualMachine *getVirtualMachineBase() { return vmBase; }

    /**
     * @brief Get the Vm Type
     * @return Reference to a constant string containing the type 
     */
    const std::string& getVmType() { return vmBase->getType(); }

    /**
     * Gets the number of instances.
     *
     * @return Number of instances.
     */
    int getNumInstances() { return vmInstances.size(); }

    /**
     * Parses of this collection of VM instances into string format.
     *
     * @param includeVmFeatures If set to \a true, the generated string will contain the features of each VM in the <b>virtualMachines</b> vector.
     * @return String containing this collection of VM instances into string format.
     */
    std::string toString(bool includeVmFeatures);

    /**
     * @brief Get the Vm Instance object
     * @param nIndex Index of the app
     * @throws std::out_of_range If the index is out of bounds
     * @return VmInstance reference
     */
    VmInstance &getVmInstance(int nIndex) { return *vmInstances.at(nIndex); }

    /**
     * @brief Returns a convenient representation for range loops
     * @return Constant vector reference which contains the instances
     */
    const std::vector<VmInstance *> &allInstances() const { return vmInstances; }

    /**
     * @brief Get the rent time
     * @return int Rent time
     */
    int getRentTime() { return nRentTime; };

    /**
     * @brief Set the rent time
     * @param nRentTime Time to rent (in hours)
     */
    void setRentTime(int nRentTime) { this->nRentTime = nRentTime; }
};

#endif /* APPINSTANCECOLLECTION_H_ */
