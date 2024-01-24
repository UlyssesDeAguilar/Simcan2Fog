#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "AppParameter.h"
#include <string>
#include <vector>

/**
 * @brief Class that represents a SIMCAN application.
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar Gudmundsson
 */
class Application
{
private:
    std::string type;                       // Type of the application in the SIMCAN repository (template)
    std::string name;                       // Name of the application in the SIMCAN repository
    std::string package;                    // NED path to the package where the app resides
    std::vector<AppParameter *> parameters; // List of parameters for this application
public:
    /**
     * Constructor that sets each attribute and creates the corresponding application instance.
     *
     * @param name Name of the application.
     * @param type Type of the application to be created.
     * @param package The package where the app resides
     */
    Application(std::string name, std::string type, std::string package);

    /**
     * Destructor
     */
    virtual ~Application();

    /**
     * Gets the name of the application.
     * @return Name of the application.
     */
    const std::string &getName() const { return name; }

    /**
     * Gets the application type.
     * @return Application type.
     */
    const std::string &getType() const { return type; }

    /**
     * Gets the application package.
     * @return Application package.
     */
    const std::string &getPackage() const { return package; }

    /**
     * @brief Gets the full path for the application model
     * @return The full NED path
     */
    const std::string getFullPath() const { return package + "." + type; }

    /**
     * Gets the parameter allocated at index position. If the index \a position does not exist, an error is raised.
     *
     * @param index Position of the parameter in the parameter vector.
     * @throws std::out_of_range If the index is out of range
     * @return Parameter at index position.
     */
    const AppParameter &getParameter(int index) const { return *parameters.at(index); }

    /**
     * @brief Gets the parameter by the name
     *
     * @param name Name of the parameter
     * @throws std::out_of_range If the name doesn't match any pattern
     * @return The parameter
     */
    const AppParameter &getParameter(std::string name) const;

    /**
     * @brief Gets all the parameters into a vector (constant) which is iterable
     * @return The vector containing all the parameters
     */
    const std::vector<AppParameter *> &getAllParameters() const { return parameters; }

    /**
     * Inserts a new parameter in the parameters vector. The new parameter is allocated at the last position in the parameter vector.
     *
     * @param param New parameter to be included in the parameter vector.
     */
    void insertParameter(AppParameter *param) { parameters.push_back(param); }

    /**
     * Returns the number of parameters of this application.
     *
     * @return Number of existing parameters in the parameter vector.
     */
    int getNumParameters() const { return parameters.size(); }

    /**
     * @brief Outputs an instance into a stream
     *
     * @param os Stream to be written
     * @param obj Application object to be written
     * @return std::ostream& Same stream as the one inserted
     */
    friend std::ostream &operator<<(std::ostream &os, const Application &obj);
};

#endif /* APPLICATION_H_ */
