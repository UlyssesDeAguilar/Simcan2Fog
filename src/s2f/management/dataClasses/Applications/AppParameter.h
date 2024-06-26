#ifndef APPPARAMETER_H_
#define APPPARAMETER_H_

#include "s2f/core/include/SIMCAN_types.h"
#include <memory>
#include <string>

/**
 * @brief Class that represents a parameter of an application.
 * @author Pablo Cerro Cañizares
 * @author Ulysses de Aguilar Gudmundsson
 *
 */
class AppParameter
{

private:
    typedef std::map<std::string, tNedType> TypeMap;
    const static TypeMap typeMap;        // Utility map for parsing the type
    std::string name;                    // Name of the parameter
    std::unique_ptr<cParImpl> parameter; // Parameter implementation

    /**
     * Converts the type of the parameter into a string.
     * @return String containing the type of the parameter.
     */
    std::string typeToString() const;

public:
    /**
     * Constructor that creates an initialized parameter.
     *
     * @param name Name of the parameter.
     * @param type Type of the parameter.
     * @param expression The expresion that will be evaluated. Can (or must) include units!
     */
    AppParameter(std::string name, std::string type, std::string expression);

    /**
     * Gets the name of the parameter.
     *
     * @return Name of the parameter.
     */
    const std::string &getName() const { return name; }

    /**
     * Gets the type of the parameter.
     *
     * @return Type of the parameter
     */
    const tNedType getType() const { return parameter->getType(); }

    /**
     * Gets the unit of the parameter.
     *
     * @return Unit of the parameter.
     */
    const char *getUnit() const { return parameter->getUnit(); }

    /**
     * Sets a new unit for the parameter.
     *
     * @param unit New unit for the parameter.
     */
    void setUnit(const char *unit) { parameter->setUnit(unit); }

    /** @name Getter functions. */
    //@{

    /**
     * Returns value as a boolean. The cParImpl type must be BOOL.
     */
    bool boolValue() const { return parameter->boolValue(nullptr); }

    /**
     * Returns value as an integer. The cParImpl type must be INT.
     * Note: Implicit conversion from DOUBLE is intentionally missing.
     */
    intpar_t intValue() const { return parameter->intValue(nullptr); }

    /**
     * Returns value as a double. The cParImpl type must be DOUBLE.
     * Note: Implicit conversion from INT is intentionally missing.
     */
    double doubleValue() const { return parameter->doubleValue(nullptr); }

    // virtual const char *stringValue(cComponent *context) const = 0;

    /**
     * Returns value as string. The cParImpl type must be STRING.
     */
    std::string stdstringValue() const { return parameter->stdstringValue(nullptr); }

    /**
     * Returns value as pointer to cXMLElement. The cParImpl type must be XML.
     */
    cXMLElement *xmlValue() const { return parameter->xmlValue(nullptr); };

    /**
     * #FIXME: Should be deleted and substituted by << operator !
     * Parses the contents of the parameter into a string.
     *
     * @return String containing the information of the parameter.
     */
    std::string toString() const;

    // TODO:
    void initModuleParameter(cPar *&param);

    /**
     * @brief Outputs an instance into a stream
     *
     * @param os Stream to be written
     * @param obj AppParameter object to be written
     * @return std::ostream& Same stream as the one inserted
     */
    friend std::ostream &operator<<(std::ostream &os, const AppParameter &obj);
};

#endif /* APPPARAMETER_H_ */
