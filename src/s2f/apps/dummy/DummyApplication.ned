package s2f.apps.dummy;
import s2f.apps.AppBase;

// Dummy application.
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2016-05-01
simple DummyApplication extends AppBase{
    
    parameters:
        string testParameter = default ("Testing dummy app!");								// Parameter for testing purposes     
         
        @class(DummyApplication);							//b
}
