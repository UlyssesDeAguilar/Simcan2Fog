package simcan2.Applications.UserApps;

// Interface that must be implemented by user applications.
// It contains the <i>private</i> parameters managed by each application.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
moduleinterface IUserApp{    
    
     parameters:    
    	double startDelay @unit(s);			// Delay time for exeuting the application (in sec.)
    	double connectionDelay @unit(s);	// Delay time to estalish connection with servers! (in sec.) 
    	bool isDistributedApp;				// True if this application is distributed or False in another case.
    	int myRank;							// Process rank   
    	string testID;						// Test description
    	string appInstance;					// Application instance. This parameter is used to distinguiss several applications that have the same name. 
		bool debugUserAppBase;				// Show log messages of UserAppBase (for deep-debugging purpose only)
		
    gates:
        input in;							// Input gate
        output out;							// Output gate
}

