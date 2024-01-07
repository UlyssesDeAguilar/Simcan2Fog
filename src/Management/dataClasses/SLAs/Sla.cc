//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "Sla.h"

Sla::Sla(std::string type)
{
    this->type = type;
}

Sla::~Sla()
{
    vmCostMap.clear();
}

void Sla::addVmCost(VirtualMachine *vmTypePtr, double base, double increase, double discount, double compensation)
{
    Sla::addVmCost(vmTypePtr->getType(), base, increase, discount, compensation);
}

void Sla::addVmCost(const std::string &vmType, double base, double increase, double discount, double compensation)
{
    if (vmCostMap.find(vmType) == vmCostMap.end())
    {
        VMCost vmCost = {
            .base = base,
            .increase = increase,
            .discount = discount,
            .compensation = compensation};
        vmCostMap[vmType] = vmCost;
    }
}

const Sla::VMCost &Sla::getVmCost(std::string vmTypeStr)
{
    if (vmCostMap.find(vmTypeStr) != vmCostMap.end())
    {
        return vmCostMap.at(vmTypeStr);
    }
    else
    {
        EV_FATAL << "#___#SLA VM not found ins SLA: " << vmTypeStr << std::endl;
        VMCost vmCost = {
            .base = 0.0,
            .increase = 0.0,
            .discount = 0.0,
            .compensation = 0.0};
        vmCostMap[vmTypeStr] = vmCost;
        return vmCostMap.at(vmTypeStr);
    }
}

std::string Sla::toString()
{
    std::ostringstream info;
    info << *this;
    return info.str();
}

std::ostream &operator<<(std::ostream &os, const Sla &obj)
{
    os << "Type:" << obj.type << "\n";
    for (const auto &e : obj.vmCostMap)
        os << "\t  + VMCost[" << e.first << "] -> " << e.second << "\n";
    return os;
}

const std::string &Sla::getType() const
{
    return type;
}

void Sla::setType(const std::string &type)
{
    this->type = type;
}

const std::map<std::string, Sla::VMCost> &Sla::getVmCostMap() const
{
    return vmCostMap;
}

void Sla::setVmCostMap(const std::map<std::string, VMCost> &vmCostMap)
{
    this->vmCostMap = vmCostMap;
}

std::ostream &operator<<(std::ostream &os, const Sla::VMCost &obj)
{
    return os << "base cost: " << obj.base
              << " - cost increase: " << obj.increase
              << " - cost discount: " << obj.discount
              << " - compensation: " << obj.compensation;
}
