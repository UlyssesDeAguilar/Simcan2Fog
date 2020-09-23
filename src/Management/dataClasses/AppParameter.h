#ifndef APPPARAMETER_H_
#define APPPARAMETER_H_

#include "Core/include/SIMCAN_types.h"
#include <string>

/**
 * Class that represent a parameter of a SIMCAN application.
 */
class AppParameter {

    private:

        /** Name of the parameter */
        std::string name;

        /** Type of the parameter */
        tNedType type;

        /** Unit of the parameter, or '-' if this parameter has no unit */
        std::string unit;

        /** Value of the parameter */
        std::string value;


    public:

        /**
         * Constructor that creates an \a empty parameter.
         */
        AppParameter();

        /**
         * Constructor that creates an initialized parameter.
         *
         * @param name Name of the parameter.
         * @param type Type of the parameter.
         * @param unit Unit of the parameter.
         * @param value Value of the parameter.
         */
        AppParameter(std::string name, tNedType type, std::string unit, std::string value);

        /**
         * Destructor.
         */
        virtual ~AppParameter();

        /**
         * Gets the name of the parameter.
         *
         * @return Name of the parameter.
         */
        std::string getName() const;

        /**
         * Set a new name for the parameter.
         *
         * @param name New name for the parameter.
         */
        void setName(std::string name);

        /**
         * Gets the type of the parameter.
         *
         * @return Type of the parameter
         */
        tNedType getType() const;

        /**
         * Sets a new type for the parameter.
         *
         * @param type New type for the parameter.
         */
        void setType(tNedType type);

        /**
         * Gets the unit of the parameter.
         *
         * @return Unit of the parameter.
         */
        const std::string& getUnit() const;

        /**
         * Sets a new unit for the parameter.
         *
         * @param unit New unit for the parameter.
         */
        void setUnit(const std::string& unit);

        /**
         * Gets the value of the parameter.
         *
         * @return Value of the parameter.
         */
        std::string getValue() const;

        /**
         * Sets a new value for the parameter.
         *
         * @param value New value for the parameter.
         */
        void setValue(std::string value);

        /**
         * Parses the contents of the parameter into a string.
         *
         * @return String containing the information of the parameter.
         */
        std::string toString ();


    private:

        /**
         * Converts the type of the parameter into a string.
         *
         * @return String containing the type of the parameter.
         */
        std::string typeToString();
};

#endif /* APPPARAMETER_H_ */
