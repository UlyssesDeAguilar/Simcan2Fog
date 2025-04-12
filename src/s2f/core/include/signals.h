#ifndef SIMCAN_EX_SIGNALS_H_
#define SIMCAN_EX_SIGNALS_H_

#include <omnetpp.h>

// TODO: Consider if these could be recorded as scalars directly instead
extern omnetpp::simsignal_t maxRam;         //!< Emmited on init stage. Maximum amount of RAM
extern omnetpp::simsignal_t maxDisk;        //!< Emmited on init stage. Maximum amount of Disk space
extern omnetpp::simsignal_t maxCores;       //!< Emmited on init stage. Maximum amount of cores
extern omnetpp::simsignal_t maxVms;         //!< Emmited on init stage. Maximum amount of VMs


extern omnetpp::simsignal_t allocatedCores; //!< Signal id for the event of cores being allocated
extern omnetpp::simsignal_t allocatedRam;   //!< Signal id for the event of RAM being allocated
extern omnetpp::simsignal_t allocatedDisk;  //!< Signal id for the event of Disk space being allocated
extern omnetpp::simsignal_t allocatedVms;   //!< Signal id for the event of VMs being allocated


#endif /* SIMCAN_EX_SIGNALS_H_ */