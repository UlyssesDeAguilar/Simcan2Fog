#include "../../../management/dataClasses/Applications/Application.h"

Application::Application(std::string name, std::string type, std::string package)
{
    this->name = name;
    this->type = type;
    this->package = package;
}

Application::~Application()
{
    parameters.clear();
}

const AppParameter &Application::getParameter(std::string name) const
{
    for (auto const& p : parameters)
        if (name.compare(p->getName()) == 0)
            return *p;
    
    throw std::out_of_range("The specified name doesn't match any parameter");
}

std::ostream &operator<<(std::ostream &os, const Application &obj)
{
    // General information
    os << "Name:    " << obj.name << "\n"
       << "Type:    " << obj.type << "\n"
       << "Package: " << obj.package << "\n"
       << "Path:    " << obj.getFullPath() << "\n"
       << "Parameters:\n";
    
    // Parameters
    for (const auto &param : obj.parameters)
        os << "\t" << *param << "\n";
    
    return os;
}
