#include "AppParameter.h"

const AppParameter::TypeMap AppParameter::typeMap = {
    {"int", tNedType::INT},
    {"long", tNedType::LONG},
    {"double", tNedType::DOUBLE},
    {"string", tNedType::STRING},
    {"xml", tNedType::XML},
    {"bool", tNedType::BOOL},
};

AppParameter::AppParameter(std::string name, std::string type, std::string expression)
{
    this->name = name;
    try
    {
        // Search the type
        auto parameterType = typeMap.at(type);
        // Create the correct implementation instance (factory method from kernel)
        parameter = std::unique_ptr<cParImpl>(cParImpl::createWithType(parameterType));

        /*
            Because we cannot retrieve from the simulation (yet) the internal schema of the
            modules we have to set the values manually when units are involved.

            That only happens with INT (= LONG) or DOUBLE types
         */
        switch (parameterType)
        {
        case tNedType::INT:
        case tNedType::DOUBLE:
        {
            // Try to parse an unit
            auto expr = new cDynamicExpression();
            expr->parse(expression.c_str());
            cExpression::Context context(nullptr);
            cNedValue v = expr->evaluate(&context);

            // Set unit and value
            parameter->setUnit(v.getUnit());
            if (parameter->getType() == tNedType::INT)
                parameter->setIntValue(v.intValue());
            else
                parameter->setDoubleValue(v.doubleValue());
            break;
        }
        case tNedType::STRING:
            parameter->setStringValue(expression.c_str());
            break;
        default:
            // Attempt parsing the expression into the correct value
            parameter->parse(expression.c_str());
            break;
        }
    }
    catch (const std::out_of_range &e)
    {
        throw cRuntimeError("App: %s -- Type %s unkown", name.c_str(), type.c_str());
    }
    catch (const std::runtime_error &e)
    {
        throw cRuntimeError("App: %s -- Error parsing %s into type %s", name.c_str(), expression.c_str(), type.c_str());
    }
}

std::string AppParameter::toString() const
{
    std::ostringstream info;
    info << this;
    return info.str();
}

std::ostream &operator<<(std::ostream &os, const AppParameter &obj)
{
    os << obj.typeToString() << " " << obj.name;

    // print unit if present
    auto unit = obj.parameter->getUnit();
    if (unit != NULL)
        os << " (" << unit << ") ";

    // Print string representation to the console
    os << " = " << obj.parameter->str();

    return os;
}

std::string AppParameter::typeToString() const
{
    std::string typeStr = "";

    switch (parameter->getType())
    {
    case tNedType::INT:
        typeStr = "int";
        break;
    case tNedType::DOUBLE:
        typeStr = "double";
        break;
    case tNedType::BOOL:
        typeStr = "bool";
        break;
    case tNedType::STRING:
        typeStr = "string";
        break;
    case tNedType::XML:
        typeStr = "xml";
        break;
    default:
        // Really shouldn't happen
        typeStr = "UnknownType";
        break;
    }

    return typeStr;
}
