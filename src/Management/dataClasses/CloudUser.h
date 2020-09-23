#ifndef CLOUDUSER_H_
#define CLOUDUSER_H_

#include "User.h"
#include "UserVmReference.h"
#include "UserAppReference.h"
#include "Management/dataClasses/include/SIMCAN2_types.h"
#include "Management/dataClasses/Sla.h"

/**
 *
 * Class that represents a user in cloud environments.
 *
 */
class CloudUser: public User {

    protected:

        /** Vector of virtual machines required by this user */
        std::vector<UserVmReference*> virtualMachines;

        /** Required applications for this user */
        std::vector<UserAppReference*> applications;

        /** User type. Name of the user in the SIMCAN repository. */
        std::string type;

        /** Number of instances of this user to be generated in the simulated environment. */
        int numInstances;

    public:

        /**
         * Constructor.
         *
         * @param type User type.
         * @param numInstances Number of instances of this user to be created in the simulation environment.
         */
        CloudUser(std::string type, int numInstances);

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
        void insertVirtualMachine(VirtualMachine *vmPtr, int numInstances, int nRentTime);

        /**
         * Gets the virtual machine at index position in the <b>virtualMachines</b> vector.
         *
         * @param index Position of the virtual machine.
         *
         * @return If the requested virtual machine is located in the vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
         */
        UserVmReference* getVirtualMachine (int index);

        /**
         * Gets the number of different virtual machines assigned to this user.
         *
         * @return Number of different virtual machines assigned to this user.
         */
        int getNumVirtualMachines();

        /**
         * Converts the information of this user into a string.
         *
         * @return String containing the information of this user.
         */
        virtual std::string toString () override;


};

#endif /* CLOUDUSER_H_ */
