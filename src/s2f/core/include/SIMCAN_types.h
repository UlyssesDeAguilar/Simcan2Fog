#ifndef _SIMCAN_TYPES_H_
#define _SIMCAN_TYPES_H_

#include <omnetpp.h>
#include <string>
#include <omnetpp/cpar.h>
#include "ServiceURL.h"

using namespace omnetpp;

/* Conventions */

// Translates to 0xFFFFFFFF, which is 255.255.255.255
static const uint32_t DC_MANAGER_LOCAL_ADDR = UINT32_MAX;
static const uint32_t DC_NETWORK_STACK = UINT32_MAX - 1;

/************************* Constants *************************/

// ---------- SIMCAN units, size and lengths ---------- //

/** Kilo byte */
static const int KB = 1024u;

/** Mega byte */
static const int MB = KB * KB;

/** Giga byte */
static const int GB = MB * KB;

/** Maximum name size */
static const int SC_NameSize = 1 * KB;

/** Maximum line size */
static const int SC_LineSize = 2 * KB;

// ---------- SIMCAN Message init ---------- //

/** Unset value */
static const int SM_UnsetValue = -1;

/** Null operation (used for initialization purposes) */
static const int SM_NullOperation = 0;

// ---------- SIMCAN constants ---------- //

/** No gate ID */
static const int SC_UnsetGateID = -5;

/** Not found */
static const int SC_NotFound = -4;

/** All OK */
#define SC_OK 0

/** Error... */
#define SC_ERROR -1

/** Type of the simulated system: cluster(static) */
#define SC_SystemClusterStatic "clusterStatic"

/** Type of the simulated system: cluster(synamic) */
#define SC_SystemClusterDynamic "clusterDynamic"

/** Type of the simulated system: cloud */
#define SC_SystemCloud "cloud"

// ---------- SIMCAN MPI delay values and messages ---------- //

// TODO: Mover a SIMCAN_Message.h???
/** Delay ratio for MPI_SEND operation */
// #define DELAY_SEND_MB 200.0
//
///** Delay ratio for MPI_RECV operation */
// #define DELAY_RECV_MB 600.0
//
///** Delay overhead for MPI_SEND operation */
// #define DELAY_SEND_TA 0.075
//
///** Delay overhead for MPI_SEND operation */
// #define DELAY_SEND_TB 0.005
//
///** Delay overhead for MPI_SEND operation */
// #define DELAY_SEND_TC 0.00025
//
///** Minimum delay overhead for MPI_SEND operations */
// #define DELAY_SEND_MINIMUM 0.000001
//
///** Delay message for MPI_SEND operations */
// #define DELAY_SEND_MSG "delay-send-message"
//
///** Delay message for MPI_RECV operations */
// #define DELAY_RECV_MSG "delay-recv-message"
//
///** Delay message for MPI_BARRIER operations */
// #define DELAY_BARRIER_MSG "delay-barrier-message"
//
///** Delay message for MPI_BARRIER operations */
// #define DELAY_BCAST_MSG "delay-bcast-message"

/** Number of bytes per sector */
// static const int SC_BytesPerSector = 512;

//
///** File not found */
// #define SIMCAN_FILE_NOT_FOUND 30001
//
///** There is not free space */
// #define SIMCAN_DISK_FULL 30002
//
///** Requested data are out of bounds */
// #define SIMCAN_DATA_OUT_OF_BOUNDS 30003
//
///** Unset message operation */
// #define UNSET_OPERATION 0
//
//
///** Empty. Not used */
// #define EMPTY -2
//
///** Full. There is no free entry. */
// #define NO_FREE_ENTRY -3
//
///** Something was not found. */
// #define NOT_FOUND -4

//
///** There is no free space... */
// #define IS_FULL -6
//
///** Default server ID */
// #define DEFAULT_SERVER_ID 0
//
///** Init Communication ID */
// #define NULL_COMM_ID 99999999
//
///** NULL Process ID */
// #define NULL_PROCESS_ID 99999999
//
///** NULL Sequence number */
// #define NULL_SEQUENCE_NUMBER 999999999
//
///** MPI compute process */
// #define MPI_COMPUTE_PROCESS "Compute"
//
///** MPI coordinator process */
// #define MPI_COORDINATOR_PROCESS "Coordinator"
//
///** Default coordinator process rank */
// #define DEFAULT_COORDINATOR_ID 0
//
///** Application name */
// #define VFS_REMOTE "Remote"
//
///** Fyle System name */
// #define VFS_LOCAL "Local"

/************************* Typedefs *************************/

/** Offset in blockList */
typedef unsigned long long int off_blockList_t;

/** Branch size in blockList (Number of contiguous blocks) */
typedef unsigned long long int size_blockList_t;

/** Typedef to maintain compatibility with Mac OS */
typedef unsigned long long int off64_T;

/** Request number */
typedef unsigned long int reqNum_t;

/** Typedef of NED basic types */
typedef cPar::Type tNedType;

/** Typedef of application states */
enum tApplicationState
{
	appWaiting,
	appRunning,
	appFinishedOK,
	appFinishedTimeout,
	appFinishedError
};

std::ostream &operator<<(std::ostream &os, tApplicationState s);

/** Typedef of VM states */
typedef enum
{
	vmIdle,
	vmAccepted,
	vmRunning,
	vmSuspended,
	vmFinished
} tVmState;

/** Typedef of CPU states */
typedef enum
{
	cpuIdle,
	cpuBusy
} tCpuState;

/************************* Structs *************************/

/**
 * Struct that represent a file branch
 */
struct FileBranch
{
	off_blockList_t offset;		/**< Offset */
	size_blockList_t numBlocks; /**< Branch size */
};
typedef struct FileBranch fileBranch;

/**
 * Structure that represents a process action.
 */
struct MPI_ProcessAction
{
	unsigned int operation;		 /** Operation type */
	simtime_t cpuTime;			 /** CPU time for processing */
	std::string fileName;		 /** File name */
	unsigned int offset;		 /** Offset */
	unsigned int sizeKB;		 /** Size (read/write) in KB */
	unsigned int firstProcessID; /** First destination process ID */
	unsigned int lastProcessID;	 /** Last destination process ID */
	unsigned int iteration;		 /** Current iteration */
	unsigned int state;			 /** Current state */
	int receivedBarrierAcks;	 /** Number of received barrier acks */
	int receivedDataAcks;		 /** Number of received data acks */
	int receivedResultAcks;		 /** Number of received result acks */
	std::string error;			 /** Error message */
};
typedef struct MPI_ProcessAction processAction;

struct GateInfo
{
	int inBaseId;  //!< Base Id of the input vector
	int outBaseId; //!< Base Id of the output vector
};

#endif /*SIMCANTYPES_H_*/
