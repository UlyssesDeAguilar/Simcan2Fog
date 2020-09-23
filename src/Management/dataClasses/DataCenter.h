#ifndef DATACENTER_H_
#define DATACENTER_H_

#include <string.h>
#include <omnetpp.h>
#include "Rack.h"

/**
 *
 * Class that contains data structures for monitoring a Data-Center.
 *
 */
class DataCenter{

    private:

        /** Name of the Data-Center */
        std::string name;

        /** Vector that contains each computing rack in the data-center */
        std::vector <Rack*> computingRacks;

        /**  Vector that contains each storage rack in the data-center */
        std::vector <Rack*> storageRacks;


    public:

       /**
        * Constructor.
        */
        DataCenter(std::string name);

       /**
        * Destructor.
        */
        virtual ~DataCenter();

       /**
        * Gets the name of this data-center.
        *
        * @return Name of this data-center.
        */
        const std::string& getName() const;

       /**
        * Adds a rack to the data-center.
        *
        * @param rack New rack to be included in the data-center.
        * @param isStorage If this parameter is set to <i>true</i>, the new rack is included in the <b>storageRacks</b> vector.
        * In other case, the new rack is included in the <b>computingRacks</b> vector.
        */
        void addRack (Rack* rack, bool isStorage);

        /**
         * Gets the rack at \a index position. If the index \a position does not exist, an error is raised.
         *
         * @param index Position of the requested rack in the corresponding vector.
         * @param isStorage Indicates if the rack is a storage rack (true) or a computing rack (false).
         * @return Requested rack at \a index position in the corresponding vector.
         */
        Rack* getRack (int index, bool isStorage);

        /**
         * Gets the number of racks for a specific type.
         *
         * @param isStorage If set to true, this method calculates the number of storage racks. In other case, this method calculates the number of computing racks.
         * @return Number of racks of the specified type.
         */
        int getNumRacks (bool isStorage);
};

#endif /* DATACENTER_H_ */
