package DcDriver;
import simcan2.Core.cSIMCAN_Core;

simple DcDriver extends cSIMCAN_Core{
    parameters:
        @class(DcDriver);
        string dcAddress;
    gates:
        input in;
        output out;
}