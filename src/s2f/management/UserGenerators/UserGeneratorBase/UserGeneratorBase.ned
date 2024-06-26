package s2f.Management.UserGenerators.UserGeneratorBase;
import s2f.Management.CloudManagerBase.CloudManagerBase;
import s2f.Management.UserGenerators.IUserGenerator;

//
// Base module for User generators. This module parses and manages cloud users.
//
simple UserGeneratorBase extends CloudManagerBase like IUserGenerator{
          
    parameters:
        bool showUserInstances = default (true);						// Show parsed user instances
    	double startDelay @unit(s) = default (5s);					// Delay time for processing users
    	bool intervalBetweenUsers = default (false);					// By default, each user arrives at a specific time
    	bool shuffleUsers = default (false);					// By default, each user arrives at a specific time
		volatile double distribution @unit(s);					// Interval gap between users arrivals if not intervalBetweenUsers. Distribution of the users otherwise.
   		bool activeCycles = default (false);		
		int numberOfCycles = default (1);						// Number of cycles
		double durationOfCycle @unit(s) = default (1d);			// Duration of a cycle
		bool isolateCycle = default (true);						// If false, interference patterns may be generated between the cycle distributions
		volatile int cycleDistribution = default (0);			// Distribution of users between cycles.
		string userTraceFilePath; 								// Path of the swf trace file
		int traceStartTime @unit(s) = default(0s);				//Start of the procesing time of the trace. Users before this time will be discarded
        int traceEndTime @unit(s) = default(999999999s);			//End of the procesing time of the trace. Users after this time will be discarded
        int userTraceMaxVms = default(999999999);               //Max number of vms per user     

   	gates:
        input fromCloudProvider;							// Input gate from Cloud provider  		
        output toCloudProvider;								// Output gates to Cloud provider
} 