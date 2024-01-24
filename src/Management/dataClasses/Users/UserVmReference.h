#ifndef USERVMREFERENCE_H_
#define USERVMREFERENCE_H_

#include <string>
#include "Management/dataClasses/VirtualMachines/VirtualMachine.h"

/**
 *
 * This class represents the number of instances of a virtual machine that is required by the user.
 *
 */
class UserVmReference
{

private:
    const VirtualMachine *vmBase; // Pointer to the virtual machine required by the user
    int numInstances;             // Number of virtual machine instances
    int nRentTime;                // Rental time (FIXME: hours ??)

public:
    /**
     * Constructor.
     *
     * @param base Pointer to the virtual machine.
     * @param numInstances Number of virtual machine instances.
     * @param nRentTime Rental time of machine instances.
     */
    UserVmReference(const VirtualMachine *base, int numInstances, int nRentTime) : vmBase(base), numInstances(numInstances), nRentTime(nRentTime){};

    /**
     * Gets the virtual machine required by the user.
     *
     * @return Virtual machine required by the user.
     */
    const VirtualMachine *getVmBase() const { return vmBase; }

    /**
     * Gets the number of instances of this virtual machine.
     *
     * @return Number of instances of this virtual machine.
     */
    int getNumInstances() const { return numInstances; }

    /**
     * Gets the renting time of instances of this virtual machine
     * @return Rent time of instances of this virtual machine
     */
    int getRentTime() const { return nRentTime; }

    /**
     * Converts the information of the virtual machine to a string.
     *
     * @return String containing the information of the required virtual machine.
     */
    std::string toString();
};

#endif /* USERVMREFERENCE_H_ */
