/*
 * RackInstances.h
 *
 * Class that contains a set of Racks of the same type
 *
 *  Created on: May 20, 2016
 *      Author: cana
 */
#ifndef RACKINSTANCES_H_
#define RACKINSTANCES_H_

#include <string>
#include <vector>
#include "Rack.h"
#include "Core/include/SIMCAN_types.h"


class RackInstances{

    private:
        RackInfo* rackInfo;         // Pointer to the RackInfo object
        std::vector<Rack*> racks;   // Vector of Racks

    public:

       /**
        * Constructor
        */
        RackInstances(RackInfo* rackInfo);

        /**
         * Inserts a new Rack
         */
        void insertNewRack (Rack* newRack);

        /**
         * Get the number of Racks
         */
        int getNumRacks();

       /**
        * Parses this RackInstances to a string
        */
        std::string toString (bool showNodeInfoFeatures);
};

#endif /* RACKINFO_H_ */
