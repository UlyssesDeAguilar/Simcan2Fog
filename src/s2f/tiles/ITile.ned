package s2f.tiles;

//
// Interface for modules that represent a micro edge scenario
// in the grand scheme of the global simulation
//
// The parameter enableRouterAsUplink helps with network routing optimization
// when the network has a considerable size and internal routing doesn't play a
// huge role in the simulation itself.
//
// @author Ulysses de Aguilar Gudmundsson
// @date 10/05/2025
// @version 2.0
moduleinterface ITile {
    parameters:
        bool enableRouterAsUplink; 	// Decides if the uplink is a connection with a internal router or with the internal switch / access point
    gates:
        inout ethernet;
}