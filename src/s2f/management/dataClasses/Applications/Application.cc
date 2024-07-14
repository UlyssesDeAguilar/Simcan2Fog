#include "Application.h"

const Application::TypeMap Application::typeMap = {
    {"int", tNedType::INT},
    {"double", tNedType::DOUBLE},
    {"string", tNedType::STRING},
    {"xml", tNedType::XML},
    {"bool", tNedType::BOOL},
};

Application::Application(std::string name, std::string type, std::string path)
{
    this->name = name;
    this->type = type;
    this->path = path;
}

std::ostream &operator<<(std::ostream &os, const Application &obj)
{
    // General information
    os << "Name:    " << obj.name << "\n"
       << "Type:    " << obj.type << "\n"
       << "Path:    " << obj.path << "\n"
       << "Parameters:\n";

    // Parameters
    for (const auto &iter : obj.parameters)
        os << "\t" << iter.first << " : " << iter.second << "\n";

    return os;
}
