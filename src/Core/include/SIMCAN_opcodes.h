#ifndef SIMCAN_OPERATION_CODES
#define SIMCAN_OPERATION_CODES

// -------------------- Message Base Length -------------------- //

/** Initial message length (1 byte)*/
static const int SM_BaseLength = 1;

// -------------------- Timer Messages -------------------- //

/** Message to wait for the connection step*/
const std::string Timer_WaitToConnect = "Timer: Wait to connect";

/** Message to wait for the execution step*/
const std::string Timer_WaitToExecute = "Timer: Wait to execute";

/** Message to wait for the resume step*/
const std::string Timer_WaitToResume = "Timer: Wait to resume";

/** Message to wait for the execution step*/
const std::string Timer_Latency = "Timer: Latency";

/** Message to wait for the execution step*/
const std::string Timer_Deadline = "Timer: Deadline";

// -------------------- Fog Operations -------------------- //
static const int SM_Fog_Write = 600;
static const int SM_Fog_Read = 601;

// -------------------- Edge Operations -------------------- //
static const int SM_Edge_Ping = 700;
static const int SM_Syscall_Req = 800;

// -------------------- User Generator -------------------- //

/** VM request */
static const int SM_VM_Req = 500;

/** VM request (response) */
static const int SM_VM_Req_Rsp = 501;

static const int SM_VM_Sub = 502;

static const int SM_VM_Notify = 503;

/** Applications submit request */
static const int SM_APP_Req = 504;

/** Applications submit request (response) */
static const int SM_APP_Rsp = 505;

// TODO: PD: No tengo NPI de donde se ponen los resultados de las operaciones, las pongo aqui.
// Y despues ya las movemos ...
static const int SM_VM_Res_Accept = 10001;
static const int SM_VM_Res_Reject = 10002;
static const int SM_APP_Sub_Accept = 10003;
static const int SM_APP_Sub_Reject = 10008;
static const int SM_APP_Sub_Timeout = 10004;
static const int SM_APP_Res_Accept = 10005;
static const int SM_APP_Res_Reject = 10006;
static const int SM_APP_Res_Timeout = 10007;

// -------------------- CPU Operations -------------------- //

/** SIMCAN Message Execution (CPU) */
static const int SM_ExecCpu = 1000;
static const int SM_AbortCpu = 1001;

/** Infinite quantum */
static const int SM_CpuInfiniteQuantum = -1;

//// ---------- IO Operations ---------- //
//
///** SIMCAN Message Open File operation */
// static const int SM_OPEN_FILE = 10;
//
///** SIMCAN Message Close File operation */
// static const int SM_CLOSE_FILE = 11;
//
///** SIMCAN Message Read File operation */
// static const int SM_READ_FILE  = 12;
//
///** SIMCAN Message Write File operation */
// static const int SM_WRITE_FILE = 13;
//
///** SIMCAN Message Create File operation */
// static const int SM_CREATE_FILE = 14;
//
///** SIMCAN Message Delete File operation */
// static const int SM_DELETE_FILE = 15;

//// ---------- MEM Operations ---------- //
//
///** SIMCAN Message Allocate memory */
// static const int SM_MEM_ALLOCATE = 50;
//
///** SIMCAN Message Release memory */
// static const int SM_MEM_RELEASE = 51;

//// ---------- Net Operations ---------- //
//
///** SIMCAN Message Create connection */
// static const int SM_CREATE_CONNECTION = 200;
//
///** SIMCAN Message Listen for connection */
// static const int SM_LISTEN_CONNECTION = 201;
//
///** SIMCAN Message Send data trough network */
// static const int SM_SEND_DATA_NET = 202;

//// ---------- MPI Operations (Trace Method) ---------- //
//
///** MPY any sender process */
////#define MPI_ANY_SENDER 4294967294U
// #define MPI_ANY_SENDER 99999999
//
///** MPI No value */
// #define MPI_NO_VALUE 999999995
//
///** MPI Master process Rank */
////#define MPI_MASTER_RANK 0
//
///** MPI Send */
// #define MPI_SEND 100
//
///** MPI Receive */
// #define MPI_RECV 200
//
///** MPI Barrier */
// #define MPI_BARRIER 300
//
///** Barrier Up */
// #define MPI_BARRIER_UP 301
//
///** Barrier Down */
// #define MPI_BARRIER_DOWN 302
//
///** MPI Broadcast */
// #define MPI_BCAST 400
//
///** MPI Scatter */
// #define MPI_SCATTER 500
//
///** MPI Gather */
// #define MPI_GATHER 600
//
///** MPI File Open */
// #define MPI_FILE_OPEN 700
//
///** MPI File Close */
// #define MPI_FILE_CLOSE 800
//
///** MPI File Create */
// #define MPI_FILE_CREATE 900
//
///** MPI File Delete */
// #define MPI_FILE_DELETE 1000
//
///** MPI File Read */
// #define MPI_FILE_READ 1100
//
///** MPI File Write */
// #define MPI_FILE_WRITE 1200
//
//
//
//
//// ---------- NFS2 Message sizes---------- //
//
///** NFS message (open Request operation) length */
// static const int SM_NFS2_OPEN_REQUEST = 36;
//
///** NFS message (open Response operation) length */
// static const int SM_NFS2_OPEN_RESPONSE = 104;
//
///** NFS message (close Request operation) length */
// static const int SM_NFS2_CLOSE_REQUEST = 36;
//
///** NFS message (close Response operation) length */
// static const int SM_NFS2_CLOSE_RESPONSE = 4;
//
///** NFS message (read Request operation) length */
// static const int SM_NFS2_READ_REQUEST = 44;
//
///** NFS message (read Response operation) length */
// static const int SM_NFS2_READ_RESPONSE = 80;
//
///** NFS message (write Request operation) length */
// static const int SM_NFS2_WRITE_REQUEST = 52;
//
///** NFS message (write Response operation) length */
// static const int SM_NFS2_WRITE_RESPONSE = 72;
//
///** NFS message (create Request operation) length */
// static const int SM_NFS2_CREATE_REQUEST = 68;
//
///** NFS message (create Response operation) length */
// static const int SM_NFS2_CREATE_RESPONSE = 104;
//
///** NFS message (delete Request operation) length */
// static const int SM_NFS2_DELETE_REQUEST = 36;
//
///** NFS message (delete Response operation) length */
// static const int SM_NFS2_DELETE_RESPONSE = 4;
//
//
//
//// ---------- SIMCAN Message Memory regions ---------- //
//
///** Memory region for code */
// static const int SM_MEMORY_REGION_CODE = 61;
//
///** Memory region for local variables */
// static const int SM_MEMORY_REGION_LOCAL_VAR = 62;
//
///** Memory region for global variables */
// static const int SM_MEMORY_REGION_GLOBAL_VAR = 63;
//
///** Memory region for dynamic variables */
// static const int SM_MEMORY_REGION_DYNAMIC_VAR = 64;
//
///** Memory error! Not enough memory! */
// static const int SM_NOT_ENOUGH_MEMORY = 65;
#endif