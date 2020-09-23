#ifndef USERAPPREFERENCE_H_
#define USERAPPREFERENCE_H_

#include <string>
#include "Application.h"

/**
 *
 * This class represents the number of instances of an application that is required by the user.
 *
 */
class UserAppReference {

    private:

        /** Pointer to the application required by the user */
        Application* appBase;

        /** Number of application instances */
        int numInstances;

    public:

        /**
         * Constructor.
         *
         * @param appPtr Pointer to the application.
         * @param numInstances Number of application instances.
         */
        UserAppReference(Application* appPtr, int numInstances);

        /**
         * Destructor.
         */
        ~UserAppReference();

        /**
         * Gets the application required by the user.
         *
         * @return Application required by the user
         */
        Application* getAppBase();

        /**
         * Gets the number of instances of this application.
         *
         * @return Number of instances of this application.
         */
        int getNumInstances() const;

        /**
         * Converts the information of the application to a string.
         *
         * @return String containing the information of the required application.
         */
        std::string toString();
};

#endif /* USERAPPREFERENCE_H_ */
