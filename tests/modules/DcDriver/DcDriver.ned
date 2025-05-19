package s2f.tests.modules.DcDriver;
import s2f.core.cSIMCAN_Core;

simple DcDriver extends cSIMCAN_Core{
    parameters:
        @class(DcDriver);
        string dcAddress;
    gates:
        input in;
        output out;
}