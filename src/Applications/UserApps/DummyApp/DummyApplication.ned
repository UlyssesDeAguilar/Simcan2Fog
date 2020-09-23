package simcan2.Applications.UserApps.DummyApp;
import simcan2.Applications.Base.UserAppBase.UserAppBase;

// Dummy application.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple DummyApplication extends UserAppBase{
    
    parameters:
        string testParameter = default ("Testing dummy app!");								// Parameter for testing purposes     
         
        @class(DummyApplication);							//b
}
