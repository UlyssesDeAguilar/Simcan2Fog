package simcan2.Management.UserGenerators.UserGeneratorBase;
import simcan2.Management.CloudManagerBase.CloudManagerBase;

//
// Base module for User generators. This module parses and manages cloud users.
//
simple UserGeneratorBase extends CloudManagerBase{
          
    parameters:
        bool showUserInstances = default (true);						// Show parsed user instances
    	double startDelay @unit(s) = default (5s);					// Delay time for processing users
    	bool intervalBetweenUsers = default (false);					// By default, each user arrives at a specific time
    	bool shuffleUsers = default (false);					// By default, each user arrives at a specific time
		volatile double distribution @unit(s);					// Interval gap between users arrivals if not intervalBetweenUsers. Distribution of the users otherwise.
        	
		int numberOfCycles = default (1);						// Number of cycles
		double durationOfCycle @unit(s) = default (1d);			// Duration of a cycle
		bool isolateCycle = default (true);						// If false, interference patterns may be generated between the cycle distributions
		volatile int cycleDistribution = default (0);			// Distribution of users between cycles.
                               
                
   	gates:
        input fromCloudProvider;							// Input gate from Cloud provider  		
        output toCloudProvider;								// Output gates to Cloud provider
} 