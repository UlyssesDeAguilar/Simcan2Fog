package s2f.apps.iot;

// Application that is executed in a single computer.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple FarmApp extends IotApplication{
    parameters:
        @class(IotApplication);
        processingMIs = default(10);				 // Number of MIs to execute in each iteration
        synthSize @unit(B) = default(70B);          // The payload that'll be sent

        //replyDelay @unit(s) = default(0.1s);         // The time the server should take to process the request
        //replySize  @unit(B) = default(20B);          // The expected size of the reply
        //vmType = default("VM_small");
        pingsThreshold = default(10);                // The number of pings where it will start processing
}