#ifndef APPINSTANCECOLLECTION_H_
#define APPINSTANCECOLLECTION_H_

#include "AppInstance.h"

/**
 *
 * Class that represents a collection of application instances to be executed by a user, where each instance represents a copy of the same application.
 *
 * Each instance is differentiated by its <i>appInstanceID</i>.
 *
 */
class AppInstanceCollection{

    private:

        /** Pointer to the object that contains the parameters of the application */
        Application* appBase;

        /** Collection of applicaiton instances to be executed */
        std::vector<AppInstance*> appInstances;


    public:

        /**
         * Constructor.
         *
         * Sets the pointer to the object that contains the parameters of the application and creates <i>numInstances</i> application instances.
         *
         * @param appPtr Pointer to the object that contains the parameters of the application.
         * @param userID User that contains this collection of applications.
         * @param numInstances Number of instances to be created.
         */
        AppInstanceCollection(Application* appPtr, std::string userID, int numInstances);

        /**
         * Destructor.
         */
        virtual ~AppInstanceCollection();

        /**
         * Gets the pointer to the object that contains the parameters of the application.
         *
         * @return Pointer to the object that contains the parameters of the application.
         */
        Application* getApplicationBase ();

        /**
         * Gets the number of instances.
         *
         * @return Number of instances.
         */
        int getNumInstances ();

        /**
         * Parses of this collection of application instances into string format.
         *
         * @return String containing this collection of application instances into string format.
         */
        std::string toString (bool showParameters);

        /**
         * Gets the pointer to an specific appInstance
         * @param nInstance
         * @return
         */
        AppInstance* getInstance(int nInstance);
    private:

        /**
         * Generates <i>numInstances</i> instances and insert them into the <b>appInstances</b> vector.
         *
         * @param userID User that contains this collection of applications.
         * @param numInstances Number of instances to be created.
         */
        void generateInstances (std::string userID, int numInstances);
};

#endif /* APPINSTANCECOLLECTION_H_ */
