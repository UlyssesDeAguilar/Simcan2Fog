import Messages.SIMCAN_Message;


///
/// SIMCAN Message for CPU requests
///
message SM_CPU_Message extends SIMCAN_Message{
    
    @customize(true);

	bool useTime = false;				// Flag that indicates this requests uses time as unit to be processed in the CPU core						
    simtime_t cpuTime = 0.0;			// CPU execution time
    
    bool useMis = false;				// Flag that indicates this requests uses MIs as unit to be processed in the CPU core
    double misToExecute = 0.0;			// Number of instructions to be executed. (in MIs)
    
    int quantum = 0;					// Quantum used in Round-Robin algorithm (measured in ticks)
    int cpuPriority = 0;				// Priority 
    
    bool isCompleted = false;			// Flag that indicates if this request has been completely processed
    
    string appInstance = "";
}


