#ifndef CLOUDUSER_H_
#define CLOUDUSER_H_

#include "User.h"
#include "UserVmReference.h"
#include "Management/dataClasses/include/SIMCAN2_types.h"
#include "Management/dataClasses/SLAs/Sla.h"

/**
 * @brief Class that represents a user in cloud environments.
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar Gudmundsson
 */
class CloudUser : public User
{
protected:
    std::vector<UserVmReference *> virtualMachines; // Vector of virtual machines required by this user

public:
    /**
     * Constructor.
     *
     * @param type User type.
     * @param numInstances Number of instances of this user to be created in the simulation environment.
     */
    CloudUser(std::string type, int numInstances) : User(type, numInstances) {}

    /**
     * Destructor.
     */
    virtual ~CloudUser();

    /**
     * Assigns a new virtual machine to this user.
     *
     * @param vmPtr Pointer to the new virtual machine assigned to this user.
     * @param numInstances Number of instances of the new virtual machine.
     * @param nRentTime Rental time of instances of the new virtual machine.
     */
    void insertVirtualMachine(const VirtualMachine *vmPtr, int numInstances, int nRentTime);

    /**
     * Gets the virtual machine at index position in the <b>virtualMachines</b> vector.
     *
     * @param index Position of the virtual machine.
     * @throws std::out_of_range If the index is off bounds
     * @return The requested VM Reference
     */
    UserVmReference *getVirtualMachine(int index) { return virtualMachines.at(index); }

    /**
     * Gets the number of different virtual machines assigned to this user.
     * @return Number of different virtual machines assigned to this user.
     */
    int getNumVirtualMachines() const { return virtualMachines.size(); }

    /**
     * @brief Returns a convenient representation of the required Virtual Machines for range loops
     * @return Constant reference to a vector containing the virtual machines
     */
    const std::vector<UserVmReference *> &allVMs() const { return virtualMachines; }

    /**
     * Converts the information of this user into a string.
     * @return String containing the information of this user.
     */
    virtual std::string toString() const override;
};

#endif /* CLOUDUSER_H_ */
