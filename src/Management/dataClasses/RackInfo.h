#ifndef RACKINFO_H_
#define RACKINFO_H_

#include <string>
#include "Core/include/SIMCAN_types.h"
#include "NodeInfo.h"

/**
 *
 * Class that contains the main features of a rack.
 *
 * This class represents the configuration of a rack in the SIMCAN repository.
 *
 */
class RackInfo{

    private:

        /** Name of this rack in the SIMCAN repository */
        std::string name;

        /** Is this node a storage rack? */
        bool storage;

        /** Number of boards */
        int numBoards;

        /** Number of nodes per board */
        int nodesPerBoard;

        /** Pointer to the NodeInfo object, which contains the configuration of the used nodes */
        NodeInfo* nodeInfo;

    public:

       /**
        * Constructor.
        *
        * @param name Name of the rack in the SIMCAN repository.
        * @param storage If this parameter is set to \a true, this node is a storage node. In other case, this node represents a computing node.
        * @param numBoards Number of boards.
        * @param nodesPerBoard Number of nodes per rack.
        * @param nodeInfo Pointer to the object containing the configuration of the nodes.
        */
        RackInfo(std::string name, bool storage, int numBoards, int nodesPerBoard, NodeInfo* nodeInfo);

        /**
         * Gets the name of the rack.
         *
         * @return Name of the rack
         */
        const std::string& getName() const;

        /**
         * Sets a new name for this rack.
         *
         * @param name New name for this rack.
         */
        void setName(const std::string& name);

        /**
         * Gets the pointer to the object containing the configuration of the used nodes.
         *
         * @return Pointer to the object containing the configuration of the used nodes
         */
        NodeInfo* getNodeInfo() const;

        /**
         * Gets a pointer to the object containing the configuration of the used nodes.
         *
         * @param nodeInfo Pointer to the object containing the configuration of the used nodes
         */
        void setNodeInfo(NodeInfo* nodeInfo);

        /**
         * Gets the number of nodes per board.
         *
         * @return Number of nodes per board
         */
        int getNodesPerBoard() const;

        /**
         * Sets a new number of nodes per board.
         *
         * @param nodesPerBoard New number of nodes per board.
         */
        void setNodesPerBoard(int nodesPerBoard);

        /**
         * Gets the number of boards.
         *
         * @return Number of boards.
         */
        int getNumBoards() const;

        /**
         * Sets a new number of boards.
         *
         * @param numBoards New number of boards.
         */
        void setNumBoards(int numBoards);

        /**
         * Gets the type of this rack.
         *
         * @return If this rack is a storage rack, \a true is returned. If this rack is a computing rack, \a false is returned.
         */
        bool isStorage() const;

        /**
         * Sets the type of this rack.
         *
         * @param storage If this parameter is set to \a true, this rack is a storage rack. In other case, this rack represents a computing rack.
         */
        void setStorage(bool storage);

        /**
         * Converts the information of this rack to string format. Usually, this method is invoked for debugging/logging purposes.
         *
         * @return String containing the configuration of this rack.
         */
        std::string toString ();

};

#endif /* RACKINFO_H_ */
