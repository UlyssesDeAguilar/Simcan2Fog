package s2f.users.edge;

import inet.applications.contract.IApp;

simple EdgeUserModelDriver like IApp {
    parameters:
        @class(EdgeUserModelDriver);

        // Signals 
        @signal[failedRequests](type=long);
        @signal[totalRequests](type=long);
    gates:
        input socketIn;
        output socketOut;
}