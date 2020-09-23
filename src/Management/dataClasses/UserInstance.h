#ifndef USERINSTANCE_H_
#define USERINSTANCE_H_

#include "AppInstanceCollection.h"
#include "CloudUser.h"
#include <string>
#include <vector>

/**
 * Class that represent a modeled user in SIMCAN.
 *
 * Basically, the instance of one user contains:
 *  - A collection of applications to be executed.
 */
class UserInstance{

    protected:

       /**
        * Name that unequivocally identifies this user instance.
        */
        std::string userID;

       /**
        * Type of this user.
        *
        * This type refers to the name that unequivocally identifies a user in the SIMCAN repository.
        */
        std::string type;

       /**
        * Number of this user.
        *
        * This number refers to the order in which this user has been created in the current simulation.
        */
        unsigned int userNumber;

       /**
        * Instance number.
        *
        * This attribute is used to differentiate several user instances of the same type.
        */
        unsigned int instanceNumber;

       /**
        * Vector of applications configured for this user.
        */
        std::vector<AppInstanceCollection*> applications;


    public:

        /**
         * Constructor.
         *
         * Generates a unique name for this user instance using the following syntax: (<i>userNumber</i>)<i>userType</i>[<i>currentInstanceIndex</i>/<i>totalUserInstances</i>]
         *
         * @param ptrUser Pointer to the user object that contains the applications to be generated.
         * @param userNumber Order in which this user has been created.
         * @param currentInstanceIndex User instance. First instance of this user must be 0.
         * @param totalUserInstances Total number of user instances to be created for this <b>userNumber</b>.
         */
        UserInstance(User *ptrUser, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances);

        /**
         * Destructor
         */
        virtual ~UserInstance();

        /**
         * Gets the instance number of this user.
         *
         * @return Instance number of this user.
         */
        unsigned int getInstanceNumber() const;

        /**
         * Gets the type of this user instance.
         *
         * @return Type of this user instance.
         */
        const std::string& getType() const;

        /**
         * Gets the ID of this user. This name unequivocally identifies this user instance in the current simulation. Generally, this attribute is used for debugging/logging purposes.
         *
         * @return Name that unequivocally identifies this user instance
         */
        const std::string& getUserID() const;

        /**
         * Gets the user number in the current simulation.
         *
         * @return User number in the current simulation
         */
        unsigned int getUserNumber() const;

        /**
         * Converts this user instance in string format.
         *
         * @param includeAppsParameters If set to \a true, the generated string will contain the parameters of each application in the <b>applications</b> vector.
         *
         * @return String containing this user instance.
         */
        virtual std::string toString (bool includeAppsParameters);


    protected:

       /**
        * Creates the corresponding instances, from the application pointed by <b>appPtr</b>, to be included in the <b>applications</b> vector.
        *
        * @param appPtr Pointer to the </b>Application</b> object that contains the main features of the generated application.
        * @param numInstances Number of instances of the generated application.
        */
       void insertNewApplicationInstances (Application* appPtr, int numInstances);
};

#endif /* USERINSTANCE_H_ */
