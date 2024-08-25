#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "s2f/core/include/SIMCAN_types.h"
#include <string>

/**
 * @brief Class that represents a SIMCAN application.
 * @author (v1) Pablo Cerro Cañizares
 * @author (v2) Ulysses de Aguilar Gudmundsson
 */
class Application
{
private:
    std::string type; // Type of the application in the SIMCAN repository (template)
    std::string name; // Name of the application in the SIMCAN repository
    std::string path; // NED path to the application
    std::map<opp_string, opp_string> parameters;

    typedef std::map<std::string, cPar::Type> TypeMap;
    const static TypeMap typeMap; // Utility map for parsing the type

public:

    /**
     * Constructor that sets each attribute and creates the corresponding application instance.
     *
     * @param name Name of the application.
     * @param type Type of the application to be created.
     * @param package The package where the app resides
     */
    Application(std::string name, std::string type, std::string path);

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
    const std::string &getPath() const { return path; }

    /**
     * @brief Gets the parameter by the name
     *
     * @param name Name of the parameter
     * @throws std::out_of_range If the name doesn't match any pattern
     * @return The parameter
     */
    const char *getParameter(const char *name) const { return parameters.at(name).c_str(); }

    /**
     * @brief Gets all the parameters into a vector (constant) which is iterable
     * @return The vector containing all the parameters
     */
    const std::map<opp_string, opp_string> &getAllParameters() const { return parameters; }

    /**
     * @brief Inserts a parameter definition
     *
     * @param name Name of the parameter
     * @param value Textual value of the parameter, will hold the same value
     */
    void insertParameter(const char *name, const char *value) { parameters[name] = value; }

    /**
     * Returns the number of parameters of this application.
     *
     * @return Number of existing parameters in the parameter vector.
     */
    int getNumParameters() const { return parameters.size(); }

    static cPar::Type strToType(const std::string &type) { return typeMap.at(type); }

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
