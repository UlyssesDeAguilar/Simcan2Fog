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

#ifndef MANAGEMENT_DATACLASSES_SLA_H_
#define MANAGEMENT_DATACLASSES_SLA_H_

#include "Core/cSIMCAN_Core.h"
#include "Management/dataClasses/include/SIMCAN2_types.h"
#include "Management/dataClasses/VirtualMachine.h"

class Sla {
public:
    Sla(std::string type);
    virtual ~Sla();

    struct VMCost {
        double base;
        double increase;
        double discount;
        double compensation;
        std::string toString();
    };

    /**
     * Add the rental costs of a VM
     * @param vmTypeStr Name of the VM type
     * @param base Rental base cost of the VM
     * @param increase Cost increase in % over the base cost. It's applied when the suer resume a VM after first rental time has expired.
     * @param discount Cost discount in % over the base cost. It's applied when the user need to wait for the resources.
     * @param compensation Cost compensation in % over the base cost. It's returned when a prioritary user cannot be served.
     */
    void addVmCost(VirtualMachine* vmTypePtr, double base, double increase = 0.0, double discount = 0.0, double compensation = 0.0);

    /**
     * Get the rental costs of a VM
     * @param vmTypeStr Name of the VM type
     * @return VMCost A struct that contains the rental costs of a VM.
     */
    const Sla::VMCost& getVmCost(std::string vmTypeStr);

    /**
     * Converts the information of this sla into a string.
     *
     * @return String containing the information of this sla.
     */
    std::string toString ();

    /**
     * Gets the type of this sla.
     *
     * @return string& Name of the sla type offered by the cloud
     */
    const std::string& getType() const;

    /**
     * Assigns the type of this sla.
     *
     * @param type Priority type of the sla
     */
    void setType(const std::string& type);

    /**
     * Gets the map that contains the list of VMs offered in the sla with their rental costs.
     *
     * @return map<string, VMCost> Map that contains the name of the VM offered as key and its rental cost in a VMCost structure.
     */
    const std::map<std::string, Sla::VMCost>& getVmCostMap() const;


    /**
     * Assigns the map that contains the list of VMs offered in the sla with their rental costs.
     *
     * @param vmCostMap map<string, VMCost> Map that contains the name of the VM offered as key and its rental cost in a VMCost structure.
     */
    void setVmCostMap(const std::map<std::string, Sla::VMCost>& vmCostMap);



private:
    /** User type. Name of the user in the SIMCAN repository. */
    std::string type;

    /** Map that contains the rental costs of each VM added to the Sla */
    std::map<std::string, VMCost> vmCostMap;

};
//
#endif /* MANAGEMENT_DATACLASSES_SLA_H_ */
