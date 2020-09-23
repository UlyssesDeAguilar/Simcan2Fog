#ifndef USERVMREFERENCE_H_
#define USERVMREFERENCE_H_

#include <string>
#include "VirtualMachine.h"

/**
 *
 * This class represents the number of instances of a virtual machine that is required by the user.
 *
 */
class UserVmReference {

    private:

        /** Pointer to the virtual machine required by the user */
        VirtualMachine* vmBase;

        /** Number of virtual machine instances */
        int numInstances;

        /** Rental time*/
        int nRentTime;

    public:

        /**
         * Constructor.
         *
         * @param vmPtr Pointer to the virtual machine.
         * @param numInstances Number of virtual machine instances.
         * @param nRentTime Rental time of machine instances.
         */
        UserVmReference(VirtualMachine* vmPtr, int numInstances, int nRentTime);

        /**
         * Destructor.
         */
        ~UserVmReference();

        /**
         * Gets the virtual machine required by the user.
         *
         * @return Virtual machine required by the user.
         */
        VirtualMachine* getVmBase();

        /**
         * Gets the number of instances of this virtual machine.
         *
         * @return Number of instances of this virtual machine.
         */
        int getNumInstances() const;

        /**
         * Gets the renting time of instances of this virtual machine
         * @return Rent time of instances of this virtual machine
         */
        int getRentTime() const;

        /**
         * Converts the information of the virtual machine to a string.
         *
         * @return String containing the information of the required virtual machine.
         */
        std::string toString();
};

#endif /* USERVMREFERENCE_H_ */
