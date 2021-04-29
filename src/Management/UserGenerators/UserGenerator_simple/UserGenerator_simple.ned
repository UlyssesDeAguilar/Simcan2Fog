package simcan2.Management.UserGenerators.UserGenerator_simple;
import simcan2.Management.UserGenerators.UserGeneratorBase.UserGeneratorBase;

//
// Implementation of a UserGenerator for cloud environments
//
simple UserGenerator_simple extends UserGeneratorBase{
    
     parameters:
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
        double maxStartTime_t1 = default (5.0);
		double nRentTime_t2 = default (5.0);
		double maxSubTime_t3 = default (5.0);
		double maxSubscriptionTime;
		double offerAcceptanceRate = default(1);
    
    @class(UserGenerator_simple);	
}
