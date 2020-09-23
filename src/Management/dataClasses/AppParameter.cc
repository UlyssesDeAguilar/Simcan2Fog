#include "AppParameter.h"

AppParameter::AppParameter() {
    name = "";
    type = NedUnset;
    unit = "";
    value = "";
}

AppParameter::AppParameter(std::string name, tNedType type, std::string unit, std::string value){
    this->name = name;
    this->type = type;
    this->unit = unit;
    this->value = value;
}

AppParameter::~AppParameter() {
}

std::string AppParameter::getName() const {
    return name;
}

void AppParameter::setName(std::string name) {
    this->name = name;
}

tNedType AppParameter::getType() const {
    return type;
}

void AppParameter::setType(tNedType type) {
    this->type = type;
}

const std::string& AppParameter::getUnit() const {
    return unit;
}

void AppParameter::setUnit(const std::string& unit) {
    this->unit = unit;
}

std::string AppParameter::getValue() const {
    return value;
}

void AppParameter::setValue(std::string value) {
    this->value = value;
}

std::string AppParameter::toString (){

    std::ostringstream info;

        info << typeToString() << " " << name << " (" << unit << ") = " << value;

    return info.str();
}


std::string AppParameter::typeToString(){

    std::string typeStr;

        // Init...
        typeStr = "";

        if (type == NedInt)
            typeStr = "int";
        else if (type == NedDouble)
            typeStr = "double";
        else if (type == NedBool)
            typeStr = "bool";
        else if (type == NedString)
            typeStr = "string";
        else
            typeStr = "UnknownType";

     return typeStr;
}
