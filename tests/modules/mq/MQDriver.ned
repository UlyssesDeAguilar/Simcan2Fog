package mq;
import s2f.Core.cSIMCAN_Core;

simple MQDriver extends cSIMCAN_Core{
    parameters:
        @class(MQDriver);
        bool kickStart = default(false);
    gates:
        input in;
        output out;
}