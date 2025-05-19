package s2f.tests.modules.mq;
import s2f.core.cSIMCAN_Core;

simple MQDriver extends cSIMCAN_Core{
    parameters:
        @class(MQDriver);
        string destinationTopic = default("");
    gates:
        input in;
        output out;
}