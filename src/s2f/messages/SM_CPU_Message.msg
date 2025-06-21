import s2f.messages.SIMCAN_Message;
import s2f.messages.Syscall;

//
// Message for handling execution between:
// - CPU module
// - CPU scheduler
// - Hypervisor/OS
// - Applications
//
// @author Pablo C. Cañizares
// @author Ulysses de Aguilar Gudmundsson
// @version 1.1
message SM_CPU_Message extends s2f::os::Syscall{
    @customize(true);
    opCode = s2f::os::SyscallCode::EXEC;
	bool useTime = false; // Flag that indicates this requests uses time as unit to be processed in the CPU core						
    simtime_t cpuTime = 0.0; // CPU execution time
    bool useMis = false; // Flag that indicates this requests uses MIs as unit to be processed in the CPU core
    double misToExecute = 0.0; // Number of instructions to be executed. (in MIs)
    int quantum = 0; // Quantum used in Round-Robin algorithm (measured in ticks)
    int cpuPriority = 0; // Priority 
    bool isCompleted = false; // Flag that indicates if this request has been completely processed
    string appInstance = ""; // Application instance that origintated the request
}
