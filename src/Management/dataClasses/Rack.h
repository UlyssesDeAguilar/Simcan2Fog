#ifndef RACK_H_
#define RACK_H_

#include <string>
#include <vector>
#include "RackInfo.h"
#include "Node.h"

/**
 *
 * Class that monitorizes the current status of a rack.
 *
 * This class represents the used and unused features of a rack.
 *
 */
class Rack{

    private:

        /** Pointer to the object that contains the configuration of this rack */
        RackInfo* rackInfo;

        /** Vector that contains the nodes of this rack */
        std::vector<Node*> nodes;


    public:

       /**
        * Constructor.
        * This method creates the nodes inside this rack and initialize them using the rackInfo object.
        *
        * @param rackInfo Pointer to the object that contains the configuration of this rack.
        */
        Rack(RackInfo* rackInfo);

       /**
        * Gets the number of nodes in this rack.
        *
        * @return Number of nodes in this rack
        */
        int getNumNodes ();

        /**
         * Gets the pointer to the object containing the configuration of this rack.
         *
         * @return Pointer to the object containing the configuration of this rack
         */
        RackInfo* getRackInfo ();

        /**
         * Gets the node at \a index position.
         *
         * @param index Position of the requested node in the <b>nodes</b> vector. If the index \a position does not exist, an error is raised.
         *
         * @return Node at \a index position
         */
        Node* getNode (int index);

};

#endif /* NodeInfo_H_ */
