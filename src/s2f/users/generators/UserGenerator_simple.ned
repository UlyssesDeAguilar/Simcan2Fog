package s2f.users.generators;

//
// Implementation of a UserGenerator for cloud environments
//
// @author Pablo C. Cañizares
// @author Adrian Bernal
// @author Ulysses de Aguilar Gudmundsson
// @version 1.3
simple UserGenerator_simple extends UserGeneratorBase
{
  parameters:
    @class(UserGenerator_simple);	
    @signal[request](type=long);
    @signal[response](type=long);
    @signal[execute](type=long);
    @signal[execute_*](type=long);
    @signal[appOK_*](type=long);
    @signal[appTimeout_*](type=long);
    @signal[subscribe](type=long);
    @signal[subscribe_*](type=long);
    @signal[notify](type=long);
    @signal[notify_*](type=long);
    @signal[timeout](type=long);
    @signal[timeout_*](type=long);
    @statistic[request](record=vector);
    @statistic[response](record=vector);
    @statistic[execute](record=vector);
    @statisticTemplate[executeSingleTemplate](record=vector);
    @statisticTemplate[appOKTemplate](record=vector);
    @statisticTemplate[appTimeoutTemplate](record=vector);
    @statistic[subscribe](record=vector);
    @statisticTemplate[subscribeSingleTemplate](record=vector);
    @statistic[notify](record=vector);
    @statisticTemplate[notifySingleTemplate](record=vector);
    @statistic[timeout](record=vector);
    @statisticTemplate[timeoutSingleTemplate](record=vector);
    
    double maxStartTime_t1 = default (5.0); // The maximum time users will accept their instance to startup
		double nRentTime_t2 = default (5.0); // The rent time of the users
		double maxSubTime_t3 = default (5.0); // Maximum time in subscription queue
		double maxSubscriptionTime; // Maximum time in subscription queue 2
		double offerAcceptanceRate = default(1); // Acceptance rate for the price of extending a VM rent time
}
