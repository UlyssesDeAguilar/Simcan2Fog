#ifndef USERINSTANCE_H_
#define USERINSTANCE_H_

#include "Management/dataClasses/Applications/AppInstanceCollection.h"
#include "CloudUser.h"
#include <string>
#include <vector>

/**
 * Class that represent a modeled user in SIMCAN.
 *
 * Basically, the instance of one user contains:
 *  - A collection of applications to be executed.
 */
class UserInstance
{
protected:
    std::string id;                                    // Name that unequivocally identifies this user instance.
    std::string type;                                  // Type of this user. This type refers to the name that unequivocally identifies a user in the SIMCAN repository
    unsigned int userNumber;                           // Number of this user. This number refers to the order in which this user has been created in the current simulation.
    unsigned int instanceNumber;                       // Instance number. This attribute is used to differentiate several user instances of the same type.
    std::vector<AppInstanceCollection *> applications; // Vector of applications configured for this user.

public:
    /**
     * Constructor.
     *
     * Generates a unique name for this user instance using the following syntax: (<i>userNumber</i>)<i>userType</i>[<i>currentInstanceIndex</i>/<i>totalUserInstances</i>]
     *
     * @param user Pointer to the user object that contains the applications to be generated.
     * @param userNumber Order in which this user has been created.
     * @param currentInstanceIndex User instance. First instance of this user must be 0.
     * @param totalUserInstances Total number of user instances to be created for this <b>userNumber</b>.
     */
    UserInstance(User *user, unsigned int userNumber, int currentInstanceIndex, int totalUserInstances);

    /**
     * Destructor
     */
    virtual ~UserInstance();

    /**
     * Gets the instance number of this user.
     *
     * @return Instance number of this user.
     */
    unsigned int getInstanceNumber() const { return instanceNumber; }

    /**
     * Gets the type of this user instance.
     *
     * @return Type of this user instance.
     */
    const std::string &getType() const { return type; }

    /**
     * Gets the ID of this user. This name unequivocally identifies this user instance in the current simulation. Generally, this attribute is used for debugging/logging purposes.
     *
     * @return Name that unequivocally identifies this user instance
     */
    const std::string &getId() const { return id; }

    /**
     * Gets the user number in the current simulation.
     *
     * @return User number in the current simulation
     */
    unsigned int getUserNumber() const { return userNumber; }

    /**
     *  Return the number of application collections
     */
    int getNumberAppCollections() { return applications.size(); }

    /**
     * Gets the number of instances of a specific App collection
     * @param nCollection The ID of the app collection to be retrieved.
     * @return Size of the collection of APP instances.
     */
    int getAppCollectionSize(int nIndex) { return applications.at(nIndex)->getNumInstances(); }

    /**
     * @brief Provides a convenient representation of the App Collections for range loops
     * @return A constant refrerence to the vector containing the App Collections
     */
    const std::vector<AppInstanceCollection *> &allAppCollections() { return applications; }

    /**
     * Converts this user instance in string format.
     *
     * @param includeAppsParameters If set to \a true, the generated string will contain the parameters of each application in the <b>applications</b> vector.
     *
     * @return String containing this user instance.
     */
    virtual std::string toString(bool includeAppsParameters);

protected:
    /**
     * Creates the corresponding instances, from the application pointed by <b>appPtr</b>, to be included in the <b>applications</b> vector.
     *
     * @param appPtr Pointer to the </b>Application</b> object that contains the main features of the generated application.
     * @param numInstances Number of instances of the generated application.
     */
    void insertNewApplicationInstances(Application *appPtr, int numInstances);
};

#endif /* USERINSTANCE_H_ */
