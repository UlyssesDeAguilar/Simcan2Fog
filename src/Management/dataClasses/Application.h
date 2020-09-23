#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "AppParameter.h"
#include <string>
#include <vector>


/**
 * Class that represents a SIMCAN application.
 *
 */
class Application{

    private:

        /** Type of the application in the SIMCAN repository (template) */
        std::string type;

        /** Name of the application in the SIMCAN repository */
        std::string appName;

        /** List of parameters for this application */
        std::vector<AppParameter*> appInstanceParameters;


    public:

       /**
        * Basic constructor that initializes each basic attribute.
        * Type and instanceName are set to empty string, and appInstanceParameters is set to empty vector.
        */
        Application();


       /**
        * Constructor that sets each attribute and creates the corresponding application instance.
        *
        * @param type Type of the application to be created.
        * @param appName Name of the application.
        */
        Application(std::string type, std::string appName);


       /**
        * Destructor
        */
        virtual ~Application();

        /**
        * Gets the name of the application.
        *
        * @return Name of the application.
        */
        const std::string& getAppName() const;

       /**
        * Sets the application name.
        *
        * @param appName Name of the application.
        */
        void setAppName(const std::string& appName);

       /**
        * Gets the application type.
        *
        * @return Application type.
        */
        const std::string& getType() const;

       /**
        * Sets the application type.
        *
        * @param type Application type.
        */
        void setType(const std::string& type);

       /**
        * Gets the parameter allocated at index position. If the index \a position does not exist, an error is raised.
        *
        * @param index Position of the parameter in the parameter vector.
        * @return Parameter at index position.
        */
        AppParameter* getParameter (int index);

       /**
        * Inserts a new parameter in the parameters vector. The new parameter is allocated at the last position in the parameter vector.
        *
        * @param param New parameter to be included in the parameter vector.
        */
        void insertParameter (AppParameter* param);

       /**
        * Returns the number of parameters of this application.
        *
        * @return Number of existing parameters in the parameter vector.
        */
        int getNumParameters ();

        /**
         * Returns the parameter given its name
         */
        AppParameter* getParameterByName(std::string strParameterName);
};

#endif /* APPLICATION_H_ */
