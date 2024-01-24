#ifndef USER_H_
#define USER_H_

#include "UserAppReference.h"
#include <string>
#include <vector>
#include <omnetpp.h>

/**
 * @brief Base class for representing a user.
 * @author Pablo Cerro Cañizares
 */
class User
{
protected:
    std::string type;                             // User type. Name of the user in the SIMCAN repository.
    int numInstances;                             // Number of instances of this user to be generated in the simulated environment.
    std::vector<UserAppReference *> applications; // Required applications for this user

public:
    /**
     * Constructor.
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
     * @return Type of this user.
     */
    const std::string &getType() const { return type; }

    /**
     * Gets the number of instances of this user.
     * @return Number of instances of this user.
     */
    int getNumInstances() const { return numInstances; }

    /**
     * Assigns a new application to this user.
     *
     * @param appPtr Pointer to the new application assigned to this user.
     * @param numInstances Number of instances of the new application.
     */
    void insertApplication(const Application *appPtr, int numInstances);

    /**
     * Gets the application at index position in the <b>applications</b> vector.
     *
     * @param index Position of the application.
     * @throws std::out_of_range If the index is off bounds
     * @return The App Reference at the position
     */
    UserAppReference *getApplication(int index) { return applications.at(index); }

    /**
     * Gets the number of applications assigned to this user.
     * @return Number of different applications, allocated in the <b>applications</b> vector, assigned to this user
     */
    int getNumApplications() const { return applications.size(); }

    /**
     * @brief Returns a convenient representation of the required Applications for range loops
     * @return Constant reference to a vector containing the applications
     */
    const std::vector<UserAppReference *> &allApplications() const { return applications; }

    /**
     * Converts the information of this user into a string.
     * @return String containing the information of this user.
     */
    virtual std::string toString() const;
};

#endif /* USER_H_ */
