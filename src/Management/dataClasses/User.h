#ifndef USER_H_
#define USER_H_

#include "UserAppReference.h"
#include <string>
#include <vector>
#include <omnetpp.h>

/**
 * Base class for representing a user.
 */
class User{

    protected:

        /** User type. Name of the user in the SIMCAN repository. */
        std::string type;

        /** Number of instances of this user to be generated in the simulated environment. */
        int numInstances;

        /** Required applications for this user */
        std::vector<UserAppReference*> applications;

    public:

        /**
         * Constructor.
         *
         * @param type User type.
         * @param numInstances Number of instances of this user to be created in the simulation environment.
         */
        User(std::string type, int numInstances);

        /**
         * Destructor.
         */
        virtual ~User();

        /**
         * Gets the type of this user.
         *
         * @return Type of tihs user.
         */
        const std::string& getType() const;

        /**
         * Gets the number of instances of this user.
         *
         * @return Number of instances of this user.
         */
        int getNumInstances() const;

        /**
         * Assigns a new application to this user.
         *
         * @param appPtr Pointer to the new application assigned to this user.
         * @param numInstances Number of instances of the new application.
         */
        void insertApplication(Application* appPtr, int numInstances);

        /**
         * Gets the application at index position in the <b>applications</b> vector.
         *
         * @param index Position of the application.
         *
         * @return If the requested application is located in the vector, then a pointer to its object is returned. In other case, \a nullptr is returned.
         */
        UserAppReference* getApplication (int index);

        /**
         * Gets the number of applications assigned to this user.
         *
         * @return Number of different applications, allocated in the <b>applications</b> vector, assigned to this user
         */
        int getNumApplications();

        /**
         * Converts the information of this user into a string.
         *
         * @return String containing the information of this user.
         */
        virtual std::string toString ();
};

#endif /* USER_H_ */
