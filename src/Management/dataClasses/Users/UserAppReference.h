#ifndef USERAPPREFERENCE_H_
#define USERAPPREFERENCE_H_

#include <string>
#include "Management/dataClasses/Applications/Application.h"

/**
 *
 * This class represents the number of instances of an application that is required by the user.
 *
 */
class UserAppReference
{

private:
    const Application *appBase; // Pointer to the application required by the user
    int numInstances;

public:
    /**
     * Constructor.
     *
     * @param appPtr Pointer to the application.
     * @param numInstances Number of application instances.
     */
    UserAppReference(const Application *appPtr, int numInstances) : appBase(appPtr), numInstances(numInstances){};

    /**
     * Gets the application required by the user.
     *
     * @return Application required by the user
     */
    const Application *getAppBase() const { return appBase; }

    /**
     * Gets the number of instances of this application.
     *
     * @return Number of instances of this application.
     */
    int getNumInstances() const { return numInstances; }

    /**
     * Converts the information of the application to a string.
     *
     * @return String containing the information of the required application.
     */
    std::string toString();
};

#endif /* USERAPPREFERENCE_H_ */
